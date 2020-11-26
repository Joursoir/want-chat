#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "user.hpp"

#define SERVER_IP "127.0.0.1"
static int port = 3030;

int main(int argc, char *argv[])
{
	initscr();
	//raw();
	noecho();

	int rows, columns;
	getmaxyx(stdscr, rows, columns); 
	if(rows != 24 || columns != 80) {
		endwin();
		printf("Please use terminal with size 24x80\n");
		return 1;
	}

	Client *user = Client::Start(SERVER_IP, port);
	if(!user) {
		endwin();
		perror("client");
		return 1;
	}

	ChatRoom *room = new ChatRoom();
	user->Run(room);
	delete room;

	endwin();
	return 0;
}