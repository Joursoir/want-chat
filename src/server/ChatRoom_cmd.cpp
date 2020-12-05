#include <stdio.h>
#include <stdlib.h>

#include "ChatRoom.hpp"
#include "ChatServer.hpp"
#include "UserInfo.hpp"

#define USE_IN_ROOM 0
#define USE_IN_LOBBY 1
#define USE_ANYWHERE 2

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
const int cmd_id_reg = 5;
const int cmd_id_login = 6;

const int cmd_count = 7;
const struct cmd_info cmd[cmd_count] = {
	{cmd_id_help, "/help", ChatRoom::Hash("/help"), USE_ANYWHERE, 0, "Usage: /help"},
	{cmd_id_create, "/create", ChatRoom::Hash("/create"), USE_IN_LOBBY, 0, "Usage: /create [pass-key]"},
	{cmd_id_join, "/join", ChatRoom::Hash("/join"), USE_IN_LOBBY, 1, "Usage: /join *id* [pass-key]"},
	{cmd_id_exit, "/exit", ChatRoom::Hash("/exit"), USE_IN_ROOM, 0, "Usage: /exit"},
	{cmd_id_rooms, "/rooms", ChatRoom::Hash("/rooms"), USE_IN_LOBBY, 0, "Usage: /rooms"}, // print all public rooms
	{cmd_id_reg, "/reg", ChatRoom::Hash("/reg"), USE_IN_LOBBY, 2, "Usage: /reg *pass* *pass*"},
	{cmd_id_login, "/login", ChatRoom::Hash("/login"), USE_IN_LOBBY, 1, "Usage: /login *pass*"}

	// IDEA: /clear - clear screen
};

void ChatRoom::HandleCommand(UserInfo *u, int count,
	char **argvar)
{
	unsigned long hash_cmd = -1;
	hash_cmd = Hash(argvar[0]);

	int what_command = -1;
	for(int i = 0; i < cmd_count; i++) {
		if(cmd[i].name_hash == hash_cmd) {
			what_command = i;
			break;
		}
	}

	if(what_command == -1)
		return u->Send("Unknown command. Use: /help");

	enum_status status = u->GetStatus();
	if(status == wait_reg && what_command != cmd_id_reg)
		return u->Send(first_reg);
	if(status == wait_login && what_command != cmd_id_login)
		return u->Send(first_login);

	// scope of command:
	if(cmd[what_command].lobby_cmd == USE_IN_ROOM && code == std_id_lobby)
		return u->Send("You can use this command only in rooms!");
	if(cmd[what_command].lobby_cmd == USE_IN_LOBBY && code != std_id_lobby)
		return u->Send("You can use this command only in lobby!");

	// right usage:
	if(cmd[what_command].min_argc > count-1)
		return u->Send(cmd[what_command].usage);

	switch(cmd[what_command].id) {
		case cmd_id_help: {
			u->Send("");
			u->Send("/help: print this message");
			u->Send("/create [pass-key]: create public/private room");
			u->Send("/join *id* [pass-key]: join to public/private room");
			u->Send("/exit: leave from room");
			u->Send("/rooms: show public rooms");
			u->Send("");
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

			u->Send("You has entered the room");
			break;
		}
		case cmd_id_exit: {
			u->Send("You has left the room");
			the_server->GotoLobby(this, u);
			break;
		}
		case cmd_id_rooms: {
			// in development
			break;
		}

		case cmd_id_reg: {
			char *pass_one = argvar[1];
			char *pass_two = argvar[2];

			if(!CheckEnterPassword(u, pass_one))
				return;
			if(strcmp(pass_one, pass_two) != 0)
				return u->Send("Passwords are not match, try again");

			// query to server (add to database)
			char *msg = new char[DB_BUFFER_SIZE];
			sprintf(msg, "INSERT INTO users (name, password) VALUES ('%s', '%s')", u->GetName(), pass_one);
			if(the_server->QueryInsert(msg) != 0) {
				perror("mariadb insert");
			}
			// query to server

			sprintf(msg, "Congratulations, %s. You registered in WhatChat.", u->GetName());
			u->Send(msg);
			delete[] msg;
			u->SetStatus(no_wait);
			break;
		}
		case cmd_id_login: {
			CONSOLE_LOG("[!] pass\n");
			char *enter_pass = argvar[1];
			const char *right_pass = u->GetPassword();

			if(strcmp(enter_pass, right_pass) != 0)
				return u->Send("Password are not right, try again");

			int msg_len = strlen("Hello, ! Glad you came back to WhatChat.");
			char *msg = new char[msg_len + max_name_len];
			sprintf(msg, "Hello, %s! Glad you came back to WhatChat.", u->GetName());
			u->Send(msg);
			delete[] msg;
			u->SetStatus(no_wait);
			break;
		}
		default: break;
	}
}

unsigned long ChatRoom::Hash(const char *str)
{
    unsigned long hash = 5381;
    char c;

    while( (c = *str++) )
        hash = ((hash << 5) + hash) + c;

    return hash;
}

char **ChatRoom::ParseToArg(const char *input, int &arrc)
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

bool ChatRoom::CheckForbiddenSymbols(const char *str)
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