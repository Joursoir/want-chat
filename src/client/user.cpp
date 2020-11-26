#include <string.h>
#include <unistd.h> 
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for iten_aton
#include <sys/types.h> // for bind, connect
#include <sys/socket.h> // for bind, connect
#include <fcntl.h>
#include <cerrno>
#include <stdio.h>

#include "user.hpp"

Client *Client::Start(const char* ip, int port)
{
	int client;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == -1) return 0;

	// remove "port sticking" aka socket in TIME_WAIT
	int opt = 1;
	setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	/* call bind optional, the system chooses
		the address automatically */

	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(port);
	if(!inet_aton(ip, &(server_adress.sin_addr))) return 0;
	
	int res = connect(client, (struct sockaddr*) &server_adress,
		sizeof(server_adress));
	if(res == -1) return 0;

	return new Client(client);
}

void Client::Run(ChatRoom *room)
{
	unsigned int usecs = 0100000;
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	do {
		this->HandleButton(room);

		int recive = read(fd, out_buffer+out_buf_used,
			sizeof(out_buffer)-out_buf_used);

		if(recive < 0) {
			if(errno != EINTR && errno != EAGAIN) 
				break;
		}
		else if(recive > 0)
			out_buf_used += recive;

		if(out_buf_used > 0) {
			/* warning: if we get a (message without '\n') > max_msg_len then
			this code will not work */
			for(int i = 0; i < out_buf_used; i++) {
				if(out_buffer[i] == '\n') {
					out_buffer[i] = 0;

					// in first char have may spec-symbol, check it:
					int spec_symbol = usually_msg;
					char *buf = out_buffer;
					if(out_buffer[0] == '#') {
						spec_symbol = system_msg;
						buf += 1;
					}

					room->AddMessage(buf, spec_symbol);
					memmove(out_buffer, out_buffer + i + 1, out_buf_used - i - 1);
					out_buf_used -= i + 1;
					break;
				}
			}
		}

		usleep(usecs);
	} while (!exit_flag);
}

void Client::HandleButton(ChatRoom *room)
{
	int key = room->InputGetch();
	switch(key)
	{
		case key_escape: {
			this->BreakLoop();
			break;
		}
		case ' '...'~': { // ascii table 32...126
			AddCharToBuffer(key);
			room->AddCharToSendMsg(key);
			break;
		}
		case '\n': { // send message
			SendMessage();
			room->InputClear();
			room->SetInputCursor(1, 1);
			break;
		}
		case key_backspace: {
			RemoveCharFromBuffer();
			room->RemoveCharFromMsg();
			break;
		}
		default: break;
	}
}

void Client::AddCharToBuffer(char ch)
{
	if(in_buf_used >= max_usermsg_len-1)
		return;

	in_buffer[in_buf_used] = ch;
	in_buf_used++;
}

void Client::RemoveCharFromBuffer()
{
	if(in_buf_used <= 0)
		return;

	in_buffer[in_buf_used] = '\0';
	in_buf_used--;
}

void Client::SendMessage()
{
	if(in_buf_used <= 0)
		return;

	in_buffer[in_buf_used] = '\n';

	write(fd, in_buffer, (in_buf_used+1)*sizeof(char));
	memset(in_buffer, 0, (in_buf_used+1)*sizeof(char)); 
	in_buf_used = 0;
}