#ifndef USER_H
#define USER_H

#include "clui.hpp"

const int max_line_length = 156;

class Client {
	int fd;
	char in_buffer[max_line_length]; // мы готовим к отправке
	int in_buf_used;

	char out_buffer[max_line_length]; // нам пришло
	int out_buf_used;

	bool exit_flag;

	Client(int i_fd)
		: fd(i_fd), in_buf_used(0), out_buf_used(0) { }
public:
	~Client() { close(fd); }

	static Client *Start(const char* ip, int port);
	void Run(ChatRoom *room);
	void HandleButton(ChatRoom *room);

	void AddCharToBuffer(char ch);
	void RemoveCharFromBuffer();
	void SendMessage();

	int getFd() const { return fd; } // not used
};

#endif