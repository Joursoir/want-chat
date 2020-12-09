#ifndef WC_CLUI_CLIENT_H
#define WC_CLUI_CLIENT_H

#include "../../const_vars.hpp"
#include "../ClientBase.hpp"

class ChatRoom;

class Client : public ClientBase {
	char in_buffer[max_usermsg_len]; // for input
	int in_buf_used;

	bool exit_flag;

	ChatRoom *room;
public:
	Client(const char* ip, int port, ChatRoom *i_room)
		: ClientBase(ip, port), in_buf_used(0),
		exit_flag(false), room(i_room) {}
	~Client() {}

	virtual void HandleActions();
	virtual void AddMessage(const char *msg, int type);
private:
	void AddCharToBuffer(char ch);
	void RemoveCharFromBuffer();
};


/*class Client {
	int fd;
	char in_buffer[max_usermsg_len]; // for input
	int in_buf_used;

	char out_buffer[max_msg_len]; // for message
	int out_buf_used;

	bool exit_flag;

	Client(int i_fd) : fd(i_fd), in_buf_used(0),
		out_buf_used(0), exit_flag(false) {}
public:
	~Client() { close(fd); }

	static Client *Start(const char* ip, int port);
	void Run(ChatRoom *room);
	void BreakLoop() { exit_flag = true; }
	void HandleButton(ChatRoom *room);

	void AddCharToBuffer(char ch);
	void RemoveCharFromBuffer();
	void SendMessage();
};*/

#endif