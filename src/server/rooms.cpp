#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rooms.hpp"
#include "chat.hpp"

// hash of commands:
#define CMD_NAME 0
#define CMD_CREATE 1
#define CMD_JOIN 2
#define CMD_EXIT 3
#define CMD_ROOMS 4

#define USE_IN_ROOM 0
#define USE_IN_LOBBY 1
#define USE_ANYWHERE 2

char **ParseToArg(const char *input, int &arrc);
unsigned long hash(const char *str);

struct cmd_info {
	char name[15];
	unsigned long name_hash;
	int lobby_cmd; // USE_IN_ROOM, USE_IN_LOBBY, USE_ANYWHERE
	int min_argc;
	char usage[64];
};

const int cmd_count = 5;
const struct cmd_info cmd[cmd_count] = {
	{"/name", hash("/name"), USE_IN_LOBBY, 1, "#Usage: /name *your_good_name*\n"},
	{"/create", hash("/create"), USE_IN_LOBBY, 0, "#Usage: /create [pass-key]\n"},
	{"/join", hash("/join"), USE_IN_LOBBY, 1, "#Usage: /join *id* [pass-key]\n"},
	{"/exit", hash("/exit"), USE_IN_ROOM, 0, "#Usage: /exit\n"},
	{"/rooms", hash("/rooms"), USE_IN_LOBBY, 0, "#Usage: /rooms\n"} // print all public rooms
	// if u add cmd, then add define too (see above) 

	// IDEA: /clear - clear screen
};

ChatRoom::~ChatRoom()
{
	while(first) {
        item *tmp = first;
        first = first->next;
        the_server->CloseConnection(tmp->s);
        delete tmp;
    }
}

void ChatRoom::SendAll(const char *msg, ChatSession *except)
{
    CONSOLE_LOG("Send message all: %s\n", msg);
    item *p;
    for(p = first; p; p = p->next)
        if(p->s != except)
            p->s->Send(msg);
}

void ChatRoom::HandleMessage(ChatSession *ses, const char *str)
{
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
	    sprintf(msg, "%s: %s\n", ses->GetName(), str);

	    this->SendAll(msg);
	    
	    delete[] msg;
	}
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
		return ses->Send("#Unknown command. Use: /help\n");

	const char onlyroom_msg[] = "#You can use this command only in rooms!\n";
	const char onlylobby_msg[] = "#You can use this command only in lobby!\n";
	const char initname_msg[] = "#First come up with name!\n";

	// scope of command:
	if(cmd[what_command].lobby_cmd == USE_IN_ROOM && code == std_id_lobby)
		return ses->Send(onlyroom_msg);
	else if(cmd[what_command].lobby_cmd == USE_IN_LOBBY && code != std_id_lobby)
		return ses->Send(onlylobby_msg);

	// right usage:
	if(cmd[what_command].min_argc > count-1)
		return ses->Send(cmd[what_command].usage);

	// name check:
	if((!ses->GetName()) && what_command != CMD_NAME)
		return ses->Send(initname_msg);

	switch(what_command) {
		case CMD_NAME: {
			int len = strlen(argvar[1]);
			if(len > max_name_len || len < 3)
				return ses->Send("#Incorrect name. Name length from 3 to 18 chars\n");

			ses->SetName(argvar[1]);
			break;
		}
		case CMD_CREATE: {
			int id = the_server->AddRoom();

			const char fcmsg[] = "#You create a room #";
			const char scmsg[] = " with password ";
			char *cmsg = new char[strlen(fcmsg)+strlen(scmsg)+4];
			sprintf(cmsg, "%s%d\n", fcmsg, id);
			ses->Send(cmsg);
			delete[] cmsg;

			the_server->ChangeSessionRoom(this, ses, id);
			break;
		}
		case CMD_JOIN: {
			int id = atoi(argvar[1]);
			bool success = false;
			if(id >= 0)
				success = the_server->ChangeSessionRoom(this, ses, id);

			if(!success)
				ses->Send("#Room with that id didn't exist\n");

			break;
		}
		case CMD_EXIT: {
			the_server->GotoLobby(this, ses);
			break;
		}
		case CMD_ROOMS: {

			break;
		}
		default: break;
	}
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
    sprintf(wmsg, "#%s%d\n", welcome_msg, code);
    ses->Send(wmsg);
    delete[] wmsg;

    char *emsg = new char[strlen(name) + sizeof(entered_msg) + 2];
    sprintf(emsg, "#%s%s\n", name, entered_msg);
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
	    sprintf(lmsg, "#%s%s\n", name, left_msg);
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

char **ParseToArg(const char *input, int &arrc)
{
	int max_argv = 5;
	arrc = 0;
	char **arr = new char*[max_argv];

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

	return arr;
}

unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    char c;

    while( (c = *str++) )
        hash = ((hash << 5) + hash) + c;

    return hash;
}