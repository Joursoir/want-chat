#include <string.h>
#include <ncurses.h>

#include "clui.hpp"

#define CHAT_WIDTH 20
#define CHAT_HEIGHT 59
#define PLAYERS_WIDTH 20
#define PLAYERS_HEIGHT 20
#define INPUT_WIDTH 4
#define INPUT_HEIGHT 80

#define MAXLEN_MSG 158

Interface_wc::Interface_wc(int num_y, int num_x, int by,
	int bx, char ch)
{
	w = newwin(num_y, num_x, by, bx);
	box(w, ch, ch);
	this->Update();
}

void Interface_wc::Hide()
{
	werase(this->GetWindow()); // clear

	this->Update();
	this->Delete();
}

////////////////////////////////////////////////////////////////////////

SelectionMenu::SelectionMenu(char const *i_title, const char**i_choises, int choises_num,
	int num_y, int num_x, int by, int bx, char ch)
	: Interface_wc(num_y, num_x, by, bx, ch), title(i_title),
	choises(i_choises), choises_number(choises_num), current_choice(0)
{ }

int SelectionMenu::Handling()
{
	WINDOW *menu = this->GetWindow();
	while(true) {
		for(int i = 0; i < choises_number; i++)
		{
			// remake:
			if(i == current_choice)
				wattron(menu, A_REVERSE);
			mvwprintw(menu, i+1, 1, choises[i]);
			wattroff(menu, A_REVERSE);
		}

		switch(wgetch(menu))
		{
			case KEY_UP:
			{
				current_choice--;
				if(current_choice < 0) current_choice = choises_number-1;
				break;
			}
			case KEY_DOWN:
			{
				current_choice++;
				if(current_choice > choises_number-1) current_choice = 0;
				break;
			}
			case '\n': return current_choice;
			default: break;
		}
	}
}

////////////////////////////////////////////////////////////////////////

ChatRoom::ChatRoom()
{
	chat = new Interface_wc(CHAT_WIDTH, CHAT_HEIGHT, 0, 0, 0);
	players = new Interface_wc(PLAYERS_WIDTH, PLAYERS_HEIGHT, 0, 60, 0);
	input = new Interface_wc(INPUT_WIDTH, INPUT_HEIGHT, 20, 0, 0);
	i_nx = 1;
	i_ny = 1;

	/* WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x) */
	/*WINDOW *chat = newwin(20, 59, 0, 0);
	box(chat, 0, 0);
	wrefresh(chat);

	WINDOW *players = newwin(20, 20, 0, 60);
	box(players, 0, 0);
	wrefresh(players);

	WINDOW *input = newwin(4, 80, 20, 0);
	box(input, 0, 0);
	wmove(input, 1, 1);
	wrefresh(input);*/
}

ChatRoom::~ChatRoom()
{
	if(chat)
		delete chat;
	if(players)
		delete players;
	if(input)
		delete input;
}

void ChatRoom::PrintMessage(const char *msg)
{
	// если msg <= **, то одна строка
	// иначе 2 строки
	WINDOW *win = chat->GetWindow();
	wmove(win, 1, 2);
	wprintw(win, msg);

	chat->Update();
}

bool ChatRoom::AddCharToSendMsg(char ch)
{
	if(i_ny == 2 && i_nx == MAXLEN_MSG/2-1)
		return 0;

	mvwaddch(input->GetWindow(), i_ny, i_nx, ch);
	i_nx++;
	if(i_nx >= MAXLEN_MSG/2) {
		if(i_ny == 1) {
			i_ny++;
			i_nx = 1;
		}
		else if(i_ny == 2) 
			i_nx--;
		wmove(input->GetWindow(), i_ny, i_nx);
	}

	input->Update();
	return 1;
}

bool ChatRoom::RemoveCharFromMsg()
{
	if(i_ny == 1 && i_nx == 1)
		return 0;

	i_nx--;
	if(i_nx < 1) {
		i_ny--;
		i_nx = MAXLEN_MSG/2-1;
	}
	mvwaddch(input->GetWindow(), i_ny, i_nx, ' ');
	wmove(input->GetWindow(), i_ny, i_nx);

	input->Update();
	return 1;
}