#include <string.h>

#include "Client.hpp"
#include "clui.hpp"

const int key_enter = 10;
const int key_escape = 27;
const int key_backspace = 127;

void Client::HandleActions()
{
	int key = room->InputGetch();
	switch(key)
	{
		case key_escape: {
			this->BreakLoop();
			break;
		}
		case ' '...'~': { // ascii table 32...126
			AddCharToBuffer(key);
			room->AddCharToSendMsg(key);
			break;
		}
		case '\n': { // send message
			if(in_buf_used <= 0)
				return;
			SendMessage(in_buffer);
			memset(in_buffer, 0, (in_buf_used+1)*sizeof(char)); 
			in_buf_used = 0;

			room->InputClear();
			room->SetInputCursor(1, 1);
			break;
		}
		case key_backspace: {
			RemoveCharFromBuffer();
			room->RemoveCharFromMsg();
			break;
		}
		default: break;
	}
}

void Client::AddMessage(const char *msg, int type)
{
	room->AddMessage(msg, type);
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