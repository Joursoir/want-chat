#include <string.h>
#include <unistd.h> 
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for iten_aton
#include <sys/types.h> // for bind, connect
#include <sys/socket.h> // for bind, connect
#include <fcntl.h>
#include <cerrno>

#include "ClientBase.hpp"

ClientBase::ClientBase(const char* ip, int port)
	: out_buf_used(0), exit_flag(false), connection(true)
{
	fd = CreateSocket(ip, port);
}

ClientBase::~ClientBase()
{
	if(fd != -1)
		close(fd);
}

int ClientBase::CreateSocket(const char* ip, int port)
{
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == -1) return -1;

	// remove "port sticking" aka socket in TIME_WAIT
	int opt = 1;
	setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	/* call bind optional, the system chooses
		the address automatically */

	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(port);
	if(!inet_aton(ip, &(server_adress.sin_addr))) return -1;
	
	int res = connect(client, (struct sockaddr*) &server_adress,
		sizeof(server_adress));
	if(res == -1) return -1;

	int flags = fcntl(client, F_GETFL, 0);
	fcntl(client, F_SETFL, flags | O_NONBLOCK);

	return client;
}

int ClientBase::Run()
{
	if(exit_flag)
		return 0;
	HandleActions();

	int recive = read(fd, out_buffer+out_buf_used,
		sizeof(out_buffer)-out_buf_used);

	if(recive < 0) {
		if(errno != EINTR && errno != EAGAIN) 
			return 0;
	}
	else if(recive > 0)
		out_buf_used += recive;
	else {
		if(connection) {
			strcpy(out_buffer, "Server closed the connection. Use ESC to exit.");
			AddMessage(out_buffer, SYSTEM_CHAR);
			connection = false;
		}
	}

	if(out_buf_used > 0) {
		/* warning: if we get a (message without '\n') > max_msg_len then
		this code will not work */
		for(int i = 0; i < out_buf_used; i++) {
			if(out_buffer[i] == '\n') {
				out_buffer[i] = 0;
	
				// in first char have may spec-symbol, check it:
				char spec_char = USUAL_CHAR;
				char *buf = out_buffer;
				switch(out_buffer[0]) {
					case SYSTEM_CHAR:
					case USERS_CHAR:
					case GONLINE_CHAR:
					case RONLINE_CHAR: {
						spec_char = out_buffer[0];
						buf += 1;
						break;
					}
					default: break;
				}

				AddMessage(buf, spec_char);
				memmove(out_buffer, out_buffer + i + 1, out_buf_used - i - 1);
				out_buf_used -= i + 1;
				break;
			}
		}
	}
	return 1;
}

void ClientBase::SendMessage(const char *msg)
{
	int len = strlen(msg) + 1;
	char *buf = new char[len];
	strcpy(buf, msg);
	buf[len-1] = '\n';

	write(fd, buf, len * sizeof(char));
}