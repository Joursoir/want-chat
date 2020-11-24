#include <string.h>
#include <unistd.h> // for close
#include <netinet/in.h> // for iten_aton, iten_ntoa, sockaddr_in
#include <arpa/inet.h> // for iten_aton, iten_ntoa
#include <sys/types.h> // for bind, connect
#include <sys/socket.h> // for bind, connect
#include <fcntl.h> // for fcntl
#include <cerrno> // for errno
#include <stdio.h>

#include "user.hpp"

const int key_esc = 4;
const int key_enter = 10;
const int key_escape = 27;
const int key_backspace = 127;

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
	unsigned int usecs = 0125000;
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	nodelay(room->GetInputWin(), true);
	curs_set(true);
	do {
		this->HandleButton(room);

		int recive = read(fd, out_buffer, sizeof(out_buffer));

		if(recive < 0) {
			if(errno != EINTR && errno != EAGAIN)
				break;
		}
		else if(recive > 0) {
			/* warning: if we get a message >= maxlen_outbuf then
			this code will not work */
			out_buffer[recive-1] = '\0'; // change '\n' to '\0'
			room->AddMessage(out_buffer);
			memset(out_buffer, 0, recive);
		}

		usleep(usecs);
	} while (!exit_flag);
}

void Client::HandleButton(ChatRoom *room)
{
	int key = wgetch(room->GetInputWin());
	switch(key)
	{
		case key_esc: {
			// #todo: exit
			break;
		}
		// ascii table 32...126
		case ' '...'~': {
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
	if(in_buf_used >= maxlen_inbuf-1)
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