#include <string.h>
#include <unistd.h> 
#ifndef _WIN32
	#include <netinet/in.h> // for sockaddr_in
	#include <arpa/inet.h> // for iten_aton
	#include <sys/types.h> // for bind, connect
	#include <sys/socket.h> // for bind, connect
#endif
#include <fcntl.h>
#include <cerrno>
#include <stdio.h>

#include "ClientBase.hpp"

ClientBase::ClientBase(const char* ip, int port)
	: out_buf_used(0), exit_flag(false), connection(true)
{
	if(InitSocket(ip, port) == -1) {
		exit(5);
#ifdef _WIN32
		fd = INVALID_SOCKET;
#else
		fd = -1;
#endif
	}
}

ClientBase::~ClientBase()
{
#ifdef _WIN32
	if(fd != INVALID_SOCKET) {
		closesocket(fd);
	}
	WSACleanup();
#else
	if(fd != -1)
		close(fd);
#endif
}

int ClientBase::ConstuctorError() const
{
#ifdef _WIN32
	return fd == INVALID_SOCKET ? 1 : 0;
#else
	return fd == -1 ? 1 : 0;
#endif
}

/* 
 * call bind optional, the system chooses
 * the address automatically
 */
int ClientBase::InitSocket(const char* ip, int port)
{
	int result;
	int opt = 1; // for remove "port sticking"
	int ifamily = AF_INET;
	int itype = SOCK_STREAM;
	int iprotocol = 0;

#ifdef _WIN32 // windows socket
	WSADATA wsaData;
	SOCKADDR_IN server_address;

	// initialize winsock:
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(result != 0) return -1;

	fd = socket(ifamily, itype, iprotocol);
	if(fd == INVALID_SOCKET)  return -1;

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt));

	server_address.sin_family = ifamily;
	server_address.sin_addr.s_addr = inet_addr(ip);
	server_address.sin_port = htons(port);
	result = connect(fd, (SOCKADDR *) &server_address, sizeof(server_address));
	if(result == SOCKET_ERROR) {
		closesocket(fd);
		return -1;
	}
#else // linux socket:
	struct sockaddr_in server_address;

	fd = socket(ifamily, itype, iprotocol);
	if(fd == -1) return -1;

	// remove "port sticking" aka socket in TIME_WAIT
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(port);
	if(!inet_aton(ip, &(server_adress.sin_addr))) {
		close(fd);
		return -1;
	}
	
	result = connect(client, (struct sockaddr*) &server_adress,
		sizeof(server_adress));
	if(result == -1) {
		close(fd);
		return -1;
	}

	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
	return 0;
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