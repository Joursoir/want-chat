#ifndef USER_H
#define USER_H

#include "clui.hpp"

const int max_line_length = 156;

class Client {
	int fd;
	char in_buffer[max_line_length]; // мы готовим к отправке
	char in_buf_used;
	char out_buffer[max_line_length]; // нам пришло
	char out_buf_used;
	bool ignoring;
	bool exit_flag;

	Client(int i_fd)
		: fd(i_fd), in_buf_used(0), out_buf_used(0), ignoring(false) { }
public:
	~Client() { close(fd); }

	static Client *Start(const char* ip, int port);
	void Run(ChatRoom *room);
	void HandleButton(ChatRoom *room);

	int getFd() const { return fd; } // not used
};

#endif