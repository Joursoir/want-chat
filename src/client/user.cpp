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
	/*int fd_stdin = STDIN_FILENO;
	const int flags = fcntl(fd_stdin, F_GETFL, 0);
	fcntl(fd_stdin, F_SETFL, flags | O_NONBLOCK);*/

	const int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	nodelay(room->GetWin(), true);
	curs_set(true);
	do {
		this->HandleButton(room);

		int recive = read(fd, out_buffer, sizeof(out_buffer));
		if(recive < 0) {
			if(errno == EINTR || errno == EAGAIN) continue;
			else break;
		}
		else if(recive > 0) {
			room->PrintMessage(out_buffer);
		}

		unsigned int usecs = 0125000;
		usleep(usecs);

		/*struct timeval timeout = { 0 };
		timeout.tv_usec = 5000000;

		fd_set rds, wrs;
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		FD_SET(fd, &rds);

		int val = read(fd_stdin, buffer, sizeof(buffer));
		if(val > 0) FD_SET(fd_stdin, &wrs);

		res = select(fd+1, &rds, &wrs, 0, &timeout);
		if(res < 0) {
			if(errno == EINTR) continue;
			else break;
		}
		else if(res > 0) {
			if(FD_ISSET(fd_stdin, &wrs)) // if client want to send any message
			{
				if(buffer[0] == '#') // exit
					exit_flag = true;
				else write(fd, buffer, strlen(buffer)*sizeof(char));
			}
			if(FD_ISSET(fd, &rds)) // if server want to send any message
			{
				read(fd, buffer, sizeof(buffer));
				//getchar();
			}
		}*/
	} while (!exit_flag);
}

void Client::HandleButton(ChatRoom *room)
{
	int key = wgetch(room->GetWin());
	switch(key)
	{
		// ascii table 32...126
		case ' '...'~': {
			room->AddCharToSendMsg(key);
			break;
		}
		case '\n': { // send message
			// some code
			break;
		}
		case key_backspace: {
			room->RemoveCharFromMsg();
			break;
		}
		default: break;
	}

}