#ifndef WC_CLUI_CLIENT_H
#define WC_CLUI_CLIENT_H

#include "../../const_vars.hpp"
#include "../ClientBase.hpp"

class WindowChat;
class WindowPlayers;
class WindowTips;
class WindowInput;

class Client : public ClientBase {
	char in_buffer[max_usermsg_len]; // for input
	int in_buf_used;

	bool exit_flag;

	WindowChat *chat;
	WindowPlayers *players;
	WindowTips *tips;
	WindowInput *input;
public:
	Client(const char* ip, int port);
	~Client();

	virtual void HandleActions();
	virtual void AddMessage(const char *msg, const char spec_char);
private:
	void AddCharToBuffer(char ch);
	void RemoveCharFromBuffer();
};

#endif