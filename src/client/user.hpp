#ifndef USER_H
#define USER_H

#include "clui.hpp"
#include "../const_vars.hpp"

const int maxlen_inbuf = max_line_len + max_name_len + 1;
const int maxlen_outbuf = max_line_len + 1;

class Client {
	int fd;
	char in_buffer[maxlen_inbuf]; // for input
	int in_buf_used;

	char out_buffer[maxlen_outbuf]; // for message

	bool exit_flag;

	Client(int i_fd)
		: fd(i_fd), in_buf_used(0) { }
public:
	~Client() { close(fd); }

	static Client *Start(const char* ip, int port);
	void Run(ChatRoom *room);
	void HandleButton(ChatRoom *room);

	void AddCharToBuffer(char ch);
	void RemoveCharFromBuffer();
	void SendMessage();
};

#endif