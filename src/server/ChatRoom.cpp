#include <stdio.h>
#include <string.h>

#include "ChatRoom.hpp"
#include "ChatServer.hpp"
#include "UserInfo.hpp"

ChatRoom::ChatRoom(ChatServer *i_server, int id, char *pass)
        : the_server(i_server), code(id)
{
	users = new StorageOfUsers();
	if(pass)
		strcpy(secret_pass, pass);
	else secret_pass[0] = 0;
}

ChatRoom::~ChatRoom()
{
	UserInfo *u = 0;
	while((u = users->Disconnect())) {
		the_server->CloseConnection(u);
	}
    delete users;
}

void ChatRoom::HandleMessage(UserInfo *u, const char *str)
{
	int status = u->GetStatus();
	if(status == wait_name) {
		if(!CheckEnterNickname(u, str)) {
			CloseSession(u);
			return;
		}
		u->SetName(str);

		// query to server
		char *msg = new char[DB_BUFFER_SIZE];
		sprintf(msg, "SELECT password FROM users WHERE name = '%s' LIMIT 1", str);
		AnswerDB *ans = the_server->QuerySelect(msg);
		if(ans) {
			DB_ROW *row = ans->GetNextRow();

			sprintf(msg, "Welcome to WantChat, %s!", u->GetName());
			u->Send(msg);
			u->Send(" ");
			u->Send("Welcome to anonymous chat in retro-style 80s.");
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

			if(row) {
				u->SetPassword((*row)[0]);
				u->SetStatus(wait_login);

				u->Send("Please, log in using /login");
			}
			else {
				u->SetStatus(wait_reg);
				u->Send("Please, create account using /reg");
			}
		}
		else {
			// handling error
			CloseSession(u);
			return;
		}
		// query to server

		delete[] msg;
		delete ans;
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

	    users->SendAllUsers(msg, 0, usual_msg);
	    
	    delete[] msg;
	}
	else {
		if(status == wait_reg) u->Send(first_reg);
		else if(status == wait_login) u->Send(first_login);
		else u->Send("In the lobby you can write only commands");
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
	users->AddUser(u);
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
    users->SendAllUsers(emsg, u);
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
	    users->SendAllUsers(lmsg, u);
	    delete[] lmsg;
	}

	users->RemoveUser(u);
	if(code != std_id_lobby && users->GetOnline() < 1)
		the_server->DeleteRoom(code);
}

void ChatRoom::CloseSession(UserInfo *u)
{
	ChatServer *serv = the_server;
	this->RemoveSession(u);
	serv->CloseConnection(u);
}

bool ChatRoom::CheckEnterNickname(UserInfo *u, const char *name)
{
	int len = strlen(name);
	if(len > max_name_len || len < min_name_len) {
		int length = strlen("Incorrect name. Name length from  to  chars");
		char *msg = new char[length + 2 + 2];
		sprintf(msg, "Incorrect name. Name length from %d to %d chars", min_name_len, max_name_len);
		u->Send(msg);
		delete[] msg;
		return 0;
	}
	if(CheckForbiddenSymbols(name)) {
		u->Send("Incorrect name. You use forbidden symbols");
		return 0;
	}
	if(the_server->IsUserOnline(name) != 0) {
		u->Send("Someone with this name is in the chat already");
		return 0;
	}
	return 1;
}

bool ChatRoom::CheckEnterPassword(UserInfo *u, const char *pass)
{
	int len = strlen(pass);
	if(len > max_player_lenpass || len < min_player_lenpass) {
		int length = strlen("Incorrect pass. Length from  to  chars");
		char *msg = new char[length + 2 + 2];
		sprintf(msg, "Incorrect pass. Length from %d to %d chars", min_player_lenpass, max_player_lenpass);
		u->Send(msg);
		delete[] msg;
		return 0;
	}
	if(CheckForbiddenSymbols(pass)) {
		u->Send("Incorrect password. You use forbidden symbols");
		return 0;
	}
	return 1;
}