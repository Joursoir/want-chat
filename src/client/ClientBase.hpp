#ifndef WC_CLIENTBASE_H
#define WC_CLIENTBASE_H

#include "../const_vars.hpp"

class ClientBase {
protected:
	int fd;
	char in_buffer[max_usermsg_len]; // for input
	int in_buf_used;

	char out_buffer[max_msg_len]; // for message
	int out_buf_used;

	bool exit_flag;
public:
	ClientBase(const char* ip, int port);
	virtual ~ClientBase();
	int ConstuctorError() const { return fd > -1 ? 0 : 1; }

	int Run();
	virtual void HandleActions() {}
	virtual void ShowMessage(const char *msg) {}
private:
	int CreateSocket(const char* ip, int port);
};

#endif