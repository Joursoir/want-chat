#include <unistd.h> // for close
#include <string>
#include <cstring>
#include <netinet/in.h> // sockaddr_in
#include <sys/types.h> // for bind, connect
#include <sys/socket.h> // for bind, connect
#include <netinet/in.h> // for iten_aton, iten_ntoa
#include <arpa/inet.h> // for iten_aton, iten_ntoa
#include <fcntl.h> // for fcntl

#define SERVER_IP "127.0.0.1"
static int port = 7777;

int main(int argc, char *argv[])
{
	int client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == -1) {
		perror("socket");
		return 1;
	}

	// remove "port sticking" aka socket in TIME_WAIT
	int opt = 1;
	setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	/* call bind optional, the system chooses
		the address automatically */

	struct sockaddr_in server_adress;
	server_adress.sin_family = AF_INET;
	server_adress.sin_port = htons(port);
	if(!inet_aton(SERVER_IP, &(server_adress.sin_addr))) {
		perror("inet_aton");
		return 1;
	}
	
	int res = connect(client, (struct sockaddr*) &server_adress,
		sizeof(server_adress));
	if(res == -1) {
		perror("connect");
		return 1;
	}

	printf("=> Connection to server %s with port number: %d\n",
		inet_ntoa(server_adress.sin_addr), port);

	// work with server
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer)); // clear

	int fd_stdin = STDIN_FILENO;
	const int flags = fcntl(fd_stdin, F_GETFL, 0);
	fcntl(fd_stdin, F_SETFL, flags | O_NONBLOCK);

	bool exit = false;
	do {
		struct timeval *timeout;
		timeout->tv_sec = 0;
		timeout->tv_usec = 5000000;

		fd_set rds, wrs;
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		FD_SET(client, &rds);

		int val = read(fd_stdin, buffer, sizeof(buffer));
		if(val > 0) FD_SET(fd_stdin, &wrs);

		res = select(client+1, &rds, &wrs, 0, timeout);
		if(res < 0) {
			if(errno == EINTR)
				continue;
			else {
				perror("select");
				break;
			}
		}

		if(res > 0) {
			if(FD_ISSET(fd_stdin, &wrs)) // if client want to send any message
			{
				if(buffer[0] == '#') // exit
					exit = true;
				else write(client, buffer, strlen(buffer)*sizeof(char));

				memset(buffer, 0, sizeof(buffer)); // clear
			}
			if(FD_ISSET(client, &rds)) // if server want to send any message
			{
				read(client, buffer, sizeof(buffer));

				printf("%s", buffer);
				getchar(); /* FIXME: if you remove this thing,
					then output will not be produced only
					if you add '\n' to the end of the line */

				memset(buffer, 0, sizeof(buffer)); // clear
			}
		}
	} while (!exit);

	printf("=> GoodBye...\n");

	close(client);
	return 0;
}