#include <stdio.h>
#include <string.h>

#include "rooms.hpp"
#include "chat.hpp"

// hash of commands:
#define CMD_NAME 210647350421
#define CMD_CREATE 229394553991880

const int cmd_num = 2;
const char *commands[cmd_num] = {
	"/name",
	"/create"
	// /rooms - print all public rooms
};

char **ParseCommand(const char *input, int &arrc);
unsigned long hash(const char *str);

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

void ChatRoom::LeaveMessage(ChatSession *except)
{
	if(it_lobby)
		return;

	const char left_msg[] = " has left the chat";
	const char *name = except->GetName();

    int len = strlen(name);
    char *lmsg = new char[len + sizeof(left_msg) + 2];
    sprintf(lmsg, "%s%s\n", name, left_msg);
    this->SendAll(lmsg, except);
    delete[] lmsg;
}

void ChatRoom::HandleMessage(ChatSession *ses, const char *str)
{
	if(str[0] == '/') { // if user sent a command
		int argc = 0;
		char **argv = 0;
		argv = ParseCommand(str, argc);

		this->HandleCommand(ses, argc, argv);
	}
	else if(!it_lobby) {
	    char *msg = new char[max_msg_len];
	    sprintf(msg, "%s: %s\n", ses->GetName(), str);

	    this->SendAll(msg);
	    
	    delete[] msg;
	}
}

void ChatRoom::HandleCommand(ChatSession *s, int cmd_counter,
	char **commands)
{
	unsigned long cmd = -1;
	cmd = hash(commands[0]);

	switch(cmd)
	{
		case CMD_NAME: {
			if(!it_lobby) {
				s->Send("You can use this command only in lobby\n");
				break;
			}

			if(cmd_counter == 1) {
				s->Send("Usage: /name your_good_name\n");
				break;
			}

			int len = strlen(commands[1]);
			if(len > max_name_len || len < 3) {
				s->Send("Incorrect name. Name length from 3 to 18 chars\n");
				break;
			}

			s->ChangeName(commands[1]);
			break;
		}
		case CMD_CREATE: {
			if(!it_lobby) {
				s->Send("You can use this command only in lobby\n");
				break;
			}
			if(!s->GetName()) {
				s->Send("Before you can use this command, use /name\n");
				break;
			}

			//int id = the_s->AddRoom();
			//the_s->AddSessionToRoom(s, id);

			break;
		}

		default: {
			s->Send("Unknown command. Use: /help\n");
			break;
		}
	}
}

void ChatRoom::AddSession(ChatSession *ses)
{
	item *p = new item;
    p->next = first;
    p->s = ses;
    first = p;
}

void ChatRoom::RemoveSession(ChatSession *ses)
{
	item **p;
	for(p = &first; *p; p = &((*p)->next)) {
		if( ((*p)->s) == ses ) {
			item *tmp = *p;
			*p = tmp->next;
			// not delete ChatSession!
			delete tmp;
			return;
		}
	}
}

void ChatRoom::CloseSession(ChatSession *ses)
{
	this->RemoveSession(ses);
	the_server->CloseConnection(ses);
}

//////////////////////////////////////////////////////////////

char **ParseCommand(const char *input, int &arrc)
{
	int max_argv = 5;
	arrc = 0;
	char **arr = new char*[max_argv];

	int start = 0;
	for(int i = 0; i < (int) strlen(input)+1; i++) {
		if(input[i] == ' ' || input[i] == '\0' || input[i] == '\n') { // end
			if(start == -1)
				continue;

			int size = i - start;
			arr[arrc] = new char[size];
			memcpy(arr[arrc], input + start, sizeof(char) * size);
			
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