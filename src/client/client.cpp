#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#include "user.hpp"

#define EXIT_BUTTON 4
#define SERVER_IP "127.0.0.1"
static int port = 7777;

int showMainMenu(int max_row, int max_col)
{
	int num_items = 5;
	const char *items[num_items] = {
		"Connect to room",
		"Create room",
		"Help",
		"About WantChat",
		"Exit"
	};

	int height_menu = num_items + 2;
	int width_menu = strlen(items[0]) + 2;

	SelectionMenu main_menu = SelectionMenu("Hello. Welcome to \
		WantChat", items, num_items, height_menu, width_menu,
		(max_row-height_menu)/2, (max_col-width_menu)/2, 0);
	main_menu.Update();

	keypad(main_menu.GetWindow(), true);
	int choice = main_menu.Handling();
	main_menu.Hide();
	return choice;
	// дескриптор будет вызван после выхода из функции неявно
}

int main(int argc, char *argv[])
{
	initscr();
	cbreak();
	noecho();
	// keypad(stdscr, TRUE);
	curs_set(false);

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

	int choice;
	while( (choice = showMainMenu(rows, columns)) != EXIT_BUTTON)
	{
		switch(choice)
		{
			case 0: {
				ChatRoom *room = new ChatRoom();
				user->Run(room);

				delete room;
				break;
			}
		}
	}

	endwin();
	return 0;
}

	/*WINDOW *chat = newwin(21, 59, 0, 0);
	box(chat, 0, 0);
	wrefresh(chat);

	WINDOW *players = newwin(21, 20, 0, 60);
	box(players, 0, 0);
	wrefresh(players);

	WINDOW *input = newwin(2, 80, 22, 0);
	//box(input, ' ', ' ');
	wmove(input, 0, 2);
	wrefresh(input);*/