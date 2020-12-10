#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../config.hpp"
#include "Client.hpp"

int main(int argc, char *argv[])
{
	initscr();
	noecho();

	int rows, columns;
	getmaxyx(stdscr, rows, columns); 
	if(rows != 24 || columns != 80) {
		endwin();
		printf("Please use terminal with size 24x80\n");
		return 1;
	}

	Client *user = new Client(SERVER_IP, SERVER_PORT);
	if(user->ConstuctorError()) {
		endwin();
		perror("server");
		return 1;
	}

	while(user->Run());
	delete user;

	endwin();
	return 0;
}