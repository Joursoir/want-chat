// CLUI - Command Line User Interface

#ifndef WC_CLUI_H
#define WC_CLUI_H

#include "WindowInterface.hpp"
#include "../../const_vars.hpp"

class WindowChat : public WindowInterface {
	struct message {
		char text[max_msg_len];
		int num_lines; // number of lines
		char spec_char;
		message *prev;
	};
	message *first;
public:
	WindowChat(int num_y, int num_x, int by, int bx, char ch)
		: WindowInterface(num_y, num_x, by, bx, ch), first(0) {}
	~WindowChat();

	void AddMessage(const char *msg, const char spec_ch);
private:
	void ChatRedraw();
	void PrintMessage(int line, message *m);
};

class WindowPlayers : public WindowInterface {
public:
	WindowPlayers(int num_y, int num_x, int by, int bx, char ch);
	~WindowPlayers() {}

	void SetPlayersList(const char *list);
};

class WindowTips : public WindowInterface {
public:
	WindowTips(int num_y, int num_x, int by, int bx, char ch);
	~WindowTips() {}

	void SetGeneralOnline(const char *online);
	void SetRoomOnline(const char *online);
};

class WindowInput : public WindowInterface {
	int i_ny, i_nx;
public:
	WindowInput(int num_y, int num_x, int by, int bx, char ch);
	~WindowInput() {}

	int GetChar();
	bool AddCharToSendMsg(char ch);
	bool RemoveCharFromMsg();
	void SetPosCursor(int y, int x);
};

#endif