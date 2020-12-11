#include <string.h>

#include "Client.hpp"
#include "clui.hpp"

#define CHAT_HEIGHT 20
#define CHAT_WIDTH 59
#define PLAYERS_WIDTH 20
#define PLAYERS_HEIGHT 12
#define TIPS_WIDTH 20
#define TIPS_HEIGHT 8
#define INPUT_HEIGHT 4
#define INPUT_WIDTH 80

const int key_enter = 10;
const int key_escape = 27;
const int key_backspace = 127;

Client::Client(const char* ip, int port)
	: ClientBase(ip, port), in_buf_used(0), exit_flag(false)
{
	chat = new WindowChat(CHAT_HEIGHT, CHAT_WIDTH, 0, 0, 0);
	players = new WindowPlayers(PLAYERS_HEIGHT, PLAYERS_WIDTH, 0, 60, 0);
	tips = new WindowTips(TIPS_HEIGHT, TIPS_WIDTH, PLAYERS_HEIGHT, 60, 0);
	input = new WindowInput(INPUT_HEIGHT, INPUT_WIDTH, 20, 0, 0);
}

Client::~Client()
{
	if(chat)
		delete chat;
	if(players)
		delete players;
	if(tips)
		delete tips;
	if(input)
		delete input;
}

void Client::HandleActions()
{
	int key = input->GetChar();
	switch(key)
	{
		case key_escape: {
			BreakLoop();
			break;
		}
		case ' '...'~': { // ascii table 32...126
			AddCharToBuffer(key);
			input->AddCharToSendMsg(key);
			break;
		}
		case '\n': { // send message
			if(in_buf_used <= 0)
				return;
			SendMessage(in_buffer);
			memset(in_buffer, 0, (in_buf_used+1)*sizeof(char)); 
			in_buf_used = 0;

			input->Clear(false);
			input->SetPosCursor(1, 1);
			break;
		}
		case key_backspace: {
			RemoveCharFromBuffer();
			input->RemoveCharFromMsg();
			break;
		}
		default: break;
	}
}

void Client::AddMessage(const char *msg, const char spec_char)
{
	if(spec_char == USERS_CHAR)
		players->SetPlayersList(msg);
	else if(spec_char == GONLINE_CHAR)
		tips->SetGeneralOnline(msg);
	else if(spec_char == RONLINE_CHAR)
		tips->SetRoomOnline(msg);
	else
		chat->AddMessage(msg, spec_char);
}

void Client::AddCharToBuffer(char ch)
{
	if(in_buf_used >= max_usermsg_len-1)
		return;

	in_buffer[in_buf_used] = ch;
	in_buf_used++;
}

void Client::RemoveCharFromBuffer()
{
	if(in_buf_used <= 0)
		return;

	in_buffer[in_buf_used] = '\0';
	in_buf_used--;
}