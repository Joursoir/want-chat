#ifndef WC_CLIENTBASE_H
#define WC_CLIENTBASE_H

#ifdef _WIN32
	#include <winsock2.h>
#endif

#include "../const_vars.hpp"

class ClientBase {
protected:
#ifdef _WIN32
	SOCKET fd;
#else
	int fd;
#endif

	char out_buffer[max_msg_len]; // for message
	int out_buf_used;

	bool exit_flag;
	bool connection;
public:
	ClientBase(const char* ip, int port);
	virtual ~ClientBase();
	int ConstuctorError() const;

	int Run();
	void BreakLoop() { exit_flag = true; }
	
	virtual void HandleActions() {}
	virtual void AddMessage(const char *msg, const char spec_char) {}
	void SendMessage(const char *msg);
private:
	int InitSocket(const char* ip, int port);
};

#endif