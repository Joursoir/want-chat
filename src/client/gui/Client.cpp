#include "Client.hpp"

void Client::AddMessage(const char *msg, int type)
{
	int len_msg = strlen(msg);
	char *source = new char[len_msg+1];
	strcpy(source, msg);

	int lines = (len_msg / oneline_len) + 1;

	for(int i = lines; i < lines_in_chat; i++) {
		chat[i-lines]->value(chat[i]->value());
		chat[i-lines]->textfont(chat[i]->textfont());
	}

	int need_print = lines;
	while(need_print > 0)
	{
		int len = strlen(source);
		int size = len > oneline_len ? oneline_len : len;

		char *str = new char[oneline_len + 1];
		int str_ptr = oneline_len * (lines - need_print);
		memcpy(str, source + str_ptr, oneline_len);
		str[size] = '\0';

		int spec = 0;
		if(type == system_msg) spec = FL_ITALIC;

		int p = lines_in_chat-need_print;
		chat[p]->textfont(STD_FONT+spec);
		chat[p]->value(str);
		delete[] str;

		need_print--;
	}

	delete[] source;
}