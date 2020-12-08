#include <string.h>
#include <unistd.h> 
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for iten_aton
#include <sys/types.h> // for bind, connect
#include <sys/socket.h> // for bind, connect
#include <fcntl.h>
#include <cerrno>

#include "ClientBase.hpp"

ClientBase::ClientBase(const char* ip, int port) : in_buf_used(0),
	out_buf_used(0), exit_flag(false)
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
		strcpy(out_buffer, "Server closed the connection. Use ESC to exit.");
		ShowMessage(out_buffer);
		//room->AddMessage(out_buffer, system_msg);
	}

	if(out_buf_used > 0) {
		/* warning: if we get a (message without '\n') > max_msg_len then
		this code will not work */
		for(int i = 0; i < out_buf_used; i++) {
			if(out_buffer[i] == '\n') {
				out_buffer[i] = 0;
	
				// in first char have may spec-symbol, check it:
				int spec_msg = usual_msg;
				char *buf = out_buffer;
				if(out_buffer[0] == system_char) {
					spec_msg = system_msg;
					buf += 1;
				}

				//room->AddMessage(buf, spec_msg);
				ShowMessage(buf);
				memmove(out_buffer, out_buffer + i + 1, out_buf_used - i - 1);
				out_buf_used -= i + 1;
				break;
			}
		}
	}

	return 1;
}