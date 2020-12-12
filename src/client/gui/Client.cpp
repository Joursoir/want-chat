#include "../dimensions_client.hpp"
#include "Client.hpp"

void Client::AddMessage(const char *msg, const char spec_char)
{
	if(spec_char == USERS_CHAR)
		UpdatePlayerList(msg);
	else if(spec_char == GONLINE_CHAR)
		SetGeneralOnline(msg);
	else if(spec_char == RONLINE_CHAR)
		SetRoomOnline(msg);
	else
		UpdateMessagesInChat(msg, spec_char);
}

void Client::UpdatePlayerList(const char *list)
{
	// clear line:
	for(int i = 0; i < PLAYERS_LINES-1; i++)
		players[i]->value("");
		
	
	int len_list = strlen(list);
	int p = 0;
	int start = 0;
	for(int i = 0; i < len_list; i++) {
        if(list[i] == ';')
        {
        	char *str = new char[max_name_len+1];
        	int size = i - start;
        	memcpy(str, list+start, size);
        	str[size] = '\0';

        	players[p]->value(str);
			start = i+1;
			p++;
			delete[] str;
        }
    }
}

void Client::SetGeneralOnline(const char *online)
{
	char *str = new char[max_name_len+1];
	sprintf(str, "Online: %s", online);
	tips[TIPS_LINE_GONLINE-1]->value(str);
	delete[] str;
}

void Client::SetRoomOnline(const char *online)
{
	char *str = new char[max_name_len+1];
	sprintf(str, "Online room: %s", online);
	tips[TIPS_LINE_RONLINE-1]->value(str);
	delete[] str;
}

void Client::UpdateMessagesInChat(const char *msg, const char spec_char)
{
	int len_msg = strlen(msg);
	char *source = new char[len_msg+1];
	strcpy(source, msg);

	int lines = (len_msg / CHAT_COLUMNS) + 1;

	for(int i = lines; i < CHAT_LINES; i++) {
		chat[i-lines]->value(chat[i]->value());
		chat[i-lines]->textfont(chat[i]->textfont());
	}

	int need_print = lines;
	while(need_print > 0)
	{
		int len = strlen(source);
		int size = len > CHAT_COLUMNS ? CHAT_COLUMNS : len;

		char *str = new char[CHAT_COLUMNS + 1];
		int str_ptr = CHAT_COLUMNS * (lines - need_print);
		memcpy(str, source + str_ptr, CHAT_COLUMNS);
		str[size] = '\0';

		int spec = 0;
		if(spec_char == SYSTEM_CHAR) spec = FL_ITALIC;
		else spec = FL_BOLD;

		int p = CHAT_LINES-need_print;
		chat[p]->textfont(STD_FONT+spec);
		chat[p]->value(str);
		delete[] str;

		need_print--;
	}

	delete[] source;
}