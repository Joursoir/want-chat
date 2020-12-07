#ifndef USER_H
#define USER_H

#include "clui.hpp"
#include "../const_vars.hpp"

class Client {
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
};

#endif