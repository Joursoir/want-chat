#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ChatRoom.hpp"
#include "ChatServer.hpp"
#include "UserInfo.hpp"

#define DB_BUFFER_SIZE 128
#define USE_IN_ROOM 0
#define USE_IN_LOBBY 1
#define USE_ANYWHERE 2

static char **ParseToArg(const char *input, int &arrc);
static unsigned long hash(const char *str);
static bool checkForbiddenSymbols(const char *str);

struct cmd_info {
	int id; // never -1 !!!
	char name[15];
	const unsigned long name_hash;
	int lobby_cmd; // USE_IN_ROOM, USE_IN_LOBBY, USE_ANYWHERE
	int min_argc;
	char usage[64];
};

const int cmd_id_help = 0;
const int cmd_id_create = 1;
const int cmd_id_join = 2;
const int cmd_id_exit = 3;
const int cmd_id_rooms = 4;

const int cmd_count = 5;
const struct cmd_info cmd[cmd_count] = {
	{cmd_id_help, "/help", hash("/help"), USE_ANYWHERE, 0, "Usage: /help"},
	{cmd_id_create, "/create", hash("/create"), USE_IN_LOBBY, 0, "Usage: /create [pass-key]"},
	{cmd_id_join, "/join", hash("/join"), USE_IN_LOBBY, 1, "Usage: /join *id* [pass-key]"},
	{cmd_id_exit, "/exit", hash("/exit"), USE_IN_ROOM, 0, "Usage: /exit"},
	{cmd_id_rooms, "/rooms", hash("/rooms"), USE_IN_LOBBY, 0, "Usage: /rooms"} // print all public rooms

	// IDEA: /clear - clear screen
};

ChatRoom::ChatRoom(ChatServer *i_server, int id, char *pass)
        : the_server(i_server), code(id), first(0)
{
	if(pass)
		strcpy(secret_pass, pass);
	else secret_pass[0] = 0;
}

ChatRoom::~ChatRoom()
{
	while(first) {
        item *tmp = first;
        first = first->next;
        the_server->CloseConnection(tmp->u);
        delete tmp;
    }
}

void ChatRoom::SendAll(const char *msg, UserInfo *except,
	const int spec_msg)
{
    CONSOLE_LOG("Send message all: %s\n", msg);
    item *p;
    for(p = first; p; p = p->next)
        if(p->u != except)
            p->u->Send(msg, spec_msg);
}

void ChatRoom::HandleMessage(UserInfo *u, const char *str)
{
	int status = u->GetStatus();
	if(status != no_wait) {
		if(status == wait_name) {
			// handle name, if OK - will send hello msg
			int len = strlen(str);
			if(len > max_name_len || len < min_name_len) {
				u->Send("Incorrect name. Name length from 3 to 18 chars");
				CloseSession(u);
			}
			if(checkForbiddenSymbols(str)) {
				u->Send("Incorrect name. You use forbidden symbols.");
				CloseSession(u);
			}

			//if(userInServer(char *nickname))
			// проверить если кто-то с данным именем..

			u->SetName(str);

			char *msg = new char[DB_BUFFER_SIZE];
			sprintf(msg, "SELECT password FROM users WHERE name = '%s' LIMIT 1", str);
			CONSOLE_LOG("%s\n", msg);
			AnswerDB *ans = the_server->QuerySelect(msg);
			if(ans) {
				DB_ROW *row = ans->GetNextRow();

				char *pass = 0;
				if(row) {
					pass = new char[max_player_lenpass];
					strcpy(pass, (*row)[0]);

					CONSOLE_LOG("account exist! \n");
				}
			}
			else CONSOLE_LOG("account noexist :( \n");
			delete[] msg;

			u->Send("Welcome to WantChat!");
		    u->Send(" ");
		    u->Send("It is anonymous chat in retro-style 80s.");
		    u->Send("Use our chat-client for more immersed.");
		    u->Send(" ");
		    u->Send("This project is open source :)");
		    u->Send("Github: github.com/Joursoir/want-chat");
		    u->Send(" ");
		    u->Send("To join to room using /join room_id");
		    u->Send("You can find rooms using /rooms");
		    u->Send(" ");
		    u->Send("For more detailed info: /help. Good chatting!");
		    u->Send(" ");

			u->SetStatus(no_wait);
		}

		return;
	}

	if(str[0] == '/') { // if user sent a command
		int argc = 0;
		char **argv = ParseToArg(str, argc);
		this->HandleCommand(u, argc, argv);

		for(int i = 0; i < argc; i++)
			delete[] argv[i];
		delete[] argv;
	}
	else if(code != std_id_lobby) {
	    char *msg = new char[max_msg_len];
	    sprintf(msg, "%s: %s", u->GetName(), str);

	    this->SendAll(msg, 0, usual_msg);
	    
	    delete[] msg;
	}
	else u->Send("In the lobby you can only write commands");
}

void ChatRoom::HandleCommand(UserInfo *u, int count,
	char **argvar)
{
	unsigned long hash_cmd = -1;
	hash_cmd = hash(argvar[0]);

	int what_command = -1;
	for(int i = 0; i < cmd_count; i++) {
		if(cmd[i].name_hash == hash_cmd) {
			what_command = i;
			break;
		}
	}

	if(what_command == -1)
		return u->Send("Unknown command. Use: /help");

	const char onlyroom_msg[] = "You can use this command only in rooms!";
	const char onlylobby_msg[] = "You can use this command only in lobby!";

	// scope of command:
	if(cmd[what_command].lobby_cmd == USE_IN_ROOM && code == std_id_lobby)
		return u->Send(onlyroom_msg);
	else if(cmd[what_command].lobby_cmd == USE_IN_LOBBY && code != std_id_lobby)
		return u->Send(onlylobby_msg);

	// right usage:
	if(cmd[what_command].min_argc > count-1)
		return u->Send(cmd[what_command].usage);

	switch(cmd[what_command].id) {
		case cmd_id_help: {
			u->Send("Help info...");
			break;
		}
		case cmd_id_create: {
			char *pass = 0;
			if(count > 1) {
				if(strlen(argvar[1]) > max_room_lenpass)
					return u->Send("Maximum length of pass equals 24");
				pass = argvar[1];
			}
			int id = the_server->AddRoom(pass);

			const char fcmsg[] = "You create a room #";
			const char scmsg[] = " with password: ";
			char *cmsg = new char[strlen(fcmsg)+3+strlen(scmsg)+max_room_lenpass];
			if(pass) sprintf(cmsg, "%s%d%s%s", fcmsg, id, scmsg, pass);
			else sprintf(cmsg, "%s%d", fcmsg, id);
			u->Send(cmsg);
			delete[] cmsg;

			the_server->ChangeSessionRoom(this, u, id, pass);
			break;
		}
		case cmd_id_join: {
			int id = atoi(argvar[1]);
			char *pass = argvar[2]; // if count == 2, then argvar[2] = 0 

			int h_status = the_server->ChangeSessionRoom(this, u, id, pass);
			if(h_status == enter_noexist)
				return u->Send("Room with that ID didn't exist");
			else if(h_status == enter_private)
				return u->Send("It is private room, join using password");
			else if(h_status == enter_uncorrect_pass)
				return u->Send("Oops, this password is not valid");

			u->Send("You has left the room");
			break;
		}
		case cmd_id_exit: {
			the_server->GotoLobby(this, u);
			break;
		}
		case cmd_id_rooms: {
			// in development
			break;
		}
		default: break;
	}
}

const char *ChatRoom::GetSecretPass()
{
	if(secret_pass[0] != 0)
		return secret_pass;
	return 0;
}

void ChatRoom::AddSession(UserInfo *u)
{
	item *p = new item;
    p->next = first;
    p->u = u;
    first = p;

    if(code == std_id_lobby)
    	return;

    const char welcome_msg[] = "Welcome to the room #";
	const char entered_msg[] = " has entered the room";

	const char *name = u->GetName();

    char *wmsg = new char[sizeof(welcome_msg) + 2];
    sprintf(wmsg, "%s%d", welcome_msg, code);
    u->Send(wmsg);
    delete[] wmsg;

    char *emsg = new char[strlen(name) + sizeof(entered_msg)];
    sprintf(emsg, "%s%s", name, entered_msg);
    this->SendAll(emsg, u);
   	delete[] emsg;

}

void ChatRoom::RemoveSession(UserInfo *u)
{
	if(code != std_id_lobby) {
		const char left_msg[] = " has left the room";
		const char *name = u->GetName();

	    int len = strlen(name);
	    char *lmsg = new char[len + sizeof(left_msg) + 2];
	    sprintf(lmsg, "%s%s", name, left_msg);
	    this->SendAll(lmsg, u);
	    delete[] lmsg;
	}

	item **p;
	for(p = &first; *p; p = &((*p)->next)) {
		if( ((*p)->u) == u ) {
			item *tmp = *p;
			*p = tmp->next;
			// not delete UserInfo!
			delete tmp;
			
			if(code != std_id_lobby && !first)
				the_server->DeleteRoom(code);

			return;
		}
	}
}

void ChatRoom::CloseSession(UserInfo *u)
{
	ChatServer *serv = the_server;
	this->RemoveSession(u);
	serv->CloseConnection(u);
}

//////////////////////////////////////////////////////////////

static char **ParseToArg(const char *input, int &arrc)
{
	int max_argv = 5;
	arrc = 0;
	char **arr = new char*[max_argv+1];

	int start = 0;
	for(int i = 0; i < (int) strlen(input)+1; i++) {
		if(input[i] == ' ' || input[i] == '\0' || input[i] == '\n') { // end
			if(start == std_id_lobby)
				continue;

			int size = i - start;
			arr[arrc] = new char[size+1];
			memcpy(arr[arrc], input + start, sizeof(char) * size);
			arr[arrc][size] = '\0';
			
			start = -1;
			arrc++;
			if(arrc == max_argv)
				break;
		}
		else if(start == -1)
			start = i;
	}

	arr[arrc] = 0;
	return arr;
}

static unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    char c;

    while( (c = *str++) )
        hash = ((hash << 5) + hash) + c;

    return hash;
}

static bool checkForbiddenSymbols(const char *str)
{
	char banned_symbols[] = {'!', '@', '#', '\'',
		'\"', '\\', '/', '^', '&', '*', ';', ','};

	int num = sizeof(banned_symbols)/sizeof(char);
	for(int i = 0; i < (int)strlen(str); i++) {
		for(int j = 0; j < num; j++) {
			if(str[i] == banned_symbols[j])
				return 1;
		}
	}

	return 0;
}