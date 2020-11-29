#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rooms.hpp"
#include "chat.hpp"

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

const int cmd_id_create = 1;
const int cmd_id_join = 2;
const int cmd_id_exit = 3;
const int cmd_id_rooms = 4;

const int cmd_count = 4;
const struct cmd_info cmd[cmd_count] = {
	{cmd_id_create, "/create", hash("/create"), USE_IN_LOBBY, 0, "Usage: /create [pass-key]"},
	{cmd_id_join, "/join", hash("/join"), USE_IN_LOBBY, 1, "Usage: /join *id* [pass-key]"},
	{cmd_id_exit, "/exit", hash("/exit"), USE_IN_ROOM, 0, "Usage: /exit"},
	{cmd_id_rooms, "/rooms", hash("/rooms"), USE_IN_LOBBY, 0, "Usage: /rooms"} // print all public rooms

	// IDEA: /clear - clear screen
};

ChatRoom::ChatRoom(Server *i_server, int id, char *pass)
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
        the_server->CloseConnection(tmp->s);
        delete tmp;
    }
}

void ChatRoom::SendAll(const char *msg, ChatSession *except,
	const int spec_msg)
{
    CONSOLE_LOG("Send message all: %s\n", msg);
    item *p;
    for(p = first; p; p = p->next)
        if(p->s != except)
            p->s->Send(msg, spec_msg);
}

void ChatRoom::HandleMessage(ChatSession *ses, const char *str)
{
	int status = ses->state;
	if(status != no_wait) {
		if(status == wait_name) {
			// handle name, if OK - will send hello msg
			int len = strlen(str);
			if(len > max_name_len || len < min_name_len) {
				ses->Send("Incorrect name. Name length from 3 to 18 chars");
				CloseSession(ses);
			}
			if(checkForbiddenSymbols(str)) {
				ses->Send("Incorrect name. You use forbidden symbols.");
				CloseSession(ses);
			}
			ses->SetName(str);

			ses->Send("Welcome to WantChat!");
		    ses->Send(" ");
		    ses->Send("It is anonymous chat in retro-style 80s.");
		    ses->Send("Use our chat-client for more immersed.");
		    ses->Send(" ");
		    ses->Send("This project is open source :)");
		    ses->Send("Github: github.com/Joursoir/want-chat");
		    ses->Send(" ");
		    ses->Send("To join to room using /join room_id");
		    ses->Send("You can find rooms using /rooms");
		    ses->Send(" ");
		    ses->Send("For more detailed info: /help. Good chatting!");
		    ses->Send(" ");

			ses->state = no_wait;
		}

		return;
	}

	if(str[0] == '/') { // if user sent a command
		int argc = 0;
		char **argv = ParseToArg(str, argc);
		this->HandleCommand(ses, argc, argv);

		for(int i = 0; i < argc; i++)
			delete[] argv[i];
		delete[] argv;
	}
	else if(code != std_id_lobby) {
	    char *msg = new char[max_msg_len];
	    sprintf(msg, "%s: %s", ses->GetName(), str);

	    this->SendAll(msg, 0, usual_msg);
	    
	    delete[] msg;
	}
	else ses->Send("In the lobby you can only write commands");
}

void ChatRoom::HandleCommand(ChatSession *ses, int count,
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
		return ses->Send("Unknown command. Use: /help");

	const char onlyroom_msg[] = "You can use this command only in rooms!";
	const char onlylobby_msg[] = "You can use this command only in lobby!";

	// scope of command:
	if(cmd[what_command].lobby_cmd == USE_IN_ROOM && code == std_id_lobby)
		return ses->Send(onlyroom_msg);
	else if(cmd[what_command].lobby_cmd == USE_IN_LOBBY && code != std_id_lobby)
		return ses->Send(onlylobby_msg);

	// right usage:
	if(cmd[what_command].min_argc > count-1)
		return ses->Send(cmd[what_command].usage);

	switch(cmd[what_command].id) {
		case cmd_id_create: {
			char *pass = 0;
			if(count > 1) {
				if(strlen(argvar[1]) > max_room_lenpass)
					return ses->Send("Maximum length of pass equals 24");
				pass = argvar[1];
			}
			int id = the_server->AddRoom(pass);

			const char fcmsg[] = "You create a room #";
			const char scmsg[] = " with password: ";
			char *cmsg = new char[strlen(fcmsg)+3+strlen(scmsg)+max_room_lenpass];
			if(pass) sprintf(cmsg, "%s%d%s%s", fcmsg, id, scmsg, pass);
			else sprintf(cmsg, "%s%d", fcmsg, id);
			ses->Send(cmsg);
			delete[] cmsg;

			the_server->ChangeSessionRoom(this, ses, id, pass);
			break;
		}
		case cmd_id_join: {
			int id = atoi(argvar[1]);
			char *pass = argvar[2]; // if count == 2, then argvar[2] = 0 

			int h_status = the_server->ChangeSessionRoom(this, ses, id, pass);
			if(h_status == enter_noexist)
				return ses->Send("Room with that ID didn't exist");
			else if(h_status == enter_private)
				return ses->Send("It is private room, join using password");
			else if(h_status == enter_uncorrect_pass)
				return ses->Send("Oops, this password is not valid");

			break;
		}
		case cmd_id_exit: {
			the_server->GotoLobby(this, ses);
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

void ChatRoom::AddSession(ChatSession *ses)
{
	item *p = new item;
    p->next = first;
    p->s = ses;
    first = p;

    if(code == std_id_lobby)
    	return;

    const char welcome_msg[] = "Welcome to the room #";
	const char entered_msg[] = " has entered the room";

	const char *name = ses->GetName();

    char *wmsg = new char[sizeof(welcome_msg) + 2];
    sprintf(wmsg, "%s%d", welcome_msg, code);
    ses->Send(wmsg);
    delete[] wmsg;

    char *emsg = new char[strlen(name) + sizeof(entered_msg)];
    sprintf(emsg, "%s%s", name, entered_msg);
    this->SendAll(emsg, ses);
   	delete[] emsg;

}

void ChatRoom::RemoveSession(ChatSession *ses)
{
	if(code != std_id_lobby) {
		const char left_msg[] = " has left the room";
		const char *name = ses->GetName();

	    int len = strlen(name);
	    char *lmsg = new char[len + sizeof(left_msg) + 2];
	    sprintf(lmsg, "%s%s", name, left_msg);
	    this->SendAll(lmsg, ses);
	    delete[] lmsg;
	}

	item **p;
	for(p = &first; *p; p = &((*p)->next)) {
		if( ((*p)->s) == ses ) {
			item *tmp = *p;
			*p = tmp->next;
			// not delete ChatSession!
			delete tmp;
			
			if(code != std_id_lobby && !first)
				the_server->DeleteRoom(code);

			return;
		}
	}
}

void ChatRoom::CloseSession(ChatSession *ses)
{
	Server *serv = the_server;
	this->RemoveSession(ses);
	serv->CloseConnection(ses);
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