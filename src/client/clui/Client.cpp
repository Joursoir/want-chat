#include <string.h>

#include "../dimensions_client.hpp"
#include "Client.hpp"
#include "clui.hpp"

const int input_lines = 2;
const int input_columns = 78;
const int key_enter = 10;
const int key_escape = 27;
const int key_backspace = 127;

Client::Client(const char* ip, int port)
	: ClientBase(ip, port), in_buf_used(0), exit_flag(false)
{
	// y+2 and x+2 for outlines
	chat = new WindowChat(CHAT_LINES+2, CHAT_COLUMNS+2, 0, 0, 0);
	players = new WindowPlayers(PLAYERS_LINES+2, PLAYERS_COLUMNS+2, 0, CHAT_COLUMNS+2+1, 0);
	tips = new WindowTips(TIPS_LINES+2, TIPS_COLUMNS+2, PLAYERS_LINES+2, CHAT_COLUMNS+2+1, 0);
	input = new WindowInput(input_lines+2, input_columns+2, CHAT_LINES+2, 0, 0);
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