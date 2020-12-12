#ifndef WC_CLIENTBASE_H
#define WC_CLIENTBASE_H

#include "../const_vars.hpp"

class ClientBase {
protected:
	int fd;

	char out_buffer[max_msg_len]; // for message
	int out_buf_used;

	bool exit_flag;
	bool connection;
public:
	ClientBase(const char* ip, int port);
	virtual ~ClientBase();
	int ConstuctorError() const { return fd > -1 ? 0 : 1; }

	int Run();
	void BreakLoop() { exit_flag = true; }
	
	virtual void HandleActions() {}
	virtual void AddMessage(const char *msg, const char spec_char) {}
	void SendMessage(const char *msg);
private:
	int CreateSocket(const char* ip, int port);
};

#endif