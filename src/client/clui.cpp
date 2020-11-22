#include <string.h>
#include <ncurses.h>

#include <unistd.h>
#include "clui.hpp"

#define CHAT_HEIGHT 20
#define CHAT_WIDTH 59
#define PLAYERS_WIDTH 20
#define PLAYERS_HEIGHT 20
#define INPUT_HEIGHT 4
#define INPUT_WIDTH 80

const int maxlen_msg_input = (INPUT_WIDTH - 2) * 2;
const int maxlen_msg_chat = (CHAT_WIDTH - 4) * 2;

Interface_wc::Interface_wc(int num_y, int num_x, int by,
	int bx, char ch) : ny(num_y), nx(num_x), beg_y(by),
	beg_x(bx), ch_line(ch)
{
	w = newwin(ny, nx, beg_y, beg_x);
	box(w, ch_line, ch_line);
	this->Update();
}

void Interface_wc::Hide()
{
	this->Clear(true);
	this->Update();
	this->Delete();
}

void Interface_wc::Clear(bool full)
{
	werase(this->GetWindow());
	if(!full)
		box(this->GetWindow(), ch_line, ch_line);
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

ChatRoom::ChatRoom() : first(0)
{
	chat = new Interface_wc(CHAT_HEIGHT, CHAT_WIDTH, 0, 0, 0);
	players = new Interface_wc(PLAYERS_HEIGHT, PLAYERS_WIDTH, 0, 60, 0);
	input = new Interface_wc(INPUT_HEIGHT, INPUT_WIDTH, 20, 0, 0);
	i_nx = 1;
	i_ny = 1;
}

ChatRoom::~ChatRoom()
{
	if(chat)
		delete chat;
	if(players)
		delete players;
	if(input)
		delete input;

	// #TODO:
	/* if(first) ... delete message */
}

void ChatRoom::AddMessage(char *msg)
{
	message *recent_msg = new message;

	int lines = 1;
	if(strlen(msg) > maxlen_msg_chat/2)
		lines = 2;

	strcpy(recent_msg->msg, msg);
	recent_msg->num_lines = lines;

	recent_msg->prev = first;
	first = recent_msg;

	ChatRedraw();
}

void ChatRoom::ChatRedraw()
{
	if(!first)
		return;

	/*wprintw(chat->GetWindow(), "redraw");
	chat->Update();
	sleep(3);*/

	chat->Clear(false);
	int available_lines = CHAT_HEIGHT - 2;
	bool remove = 0;
	message *tmp;

	for(tmp = first; tmp; tmp = tmp->prev) {
		if(available_lines >= tmp->num_lines) {
			PrintMessage(available_lines, tmp);
			available_lines -= tmp->num_lines;
		}
		// #TODO:
		/* delete */

	}

	chat->Update();
}

void ChatRoom::PrintMessage(int line, message *m)
{
	WINDOW *win = this->GetChatWin();
	if(m->num_lines == 1) {
		wmove(win, line, 2);
		wprintw(win, m->msg);
	}
	else {
		wmove(win, line-1, 2);
		// print a half of message:
		int len_fline = maxlen_msg_chat/2;


		char *temp = new char[len_fline+1];
		// memccpy(void *restrict s1, const void *restrict s2, int c, size_t n);
		memcpy(temp, m->msg, sizeof(char) * len_fline);
		temp[len_fline] = '\0';
		wprintw(win, temp);

		int last_len_msg = strlen(m->msg) - len_fline;
		memcpy(temp, m->msg, sizeof(char) * (last_len_msg+1));
		temp[last_len_msg] = '\0';
		wmove(win, line, 2);
		wprintw(win, temp);

		delete[] temp;
	}
}

bool ChatRoom::AddCharToSendMsg(char ch)
{
	if(i_ny == 2 && i_nx == maxlen_msg_input/2-1)
		return 0;

	mvwaddch(input->GetWindow(), i_ny, i_nx, ch);
	i_nx++;
	if(i_nx >= maxlen_msg_input/2-1) {
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
		i_nx = maxlen_msg_input/2-1;
	}
	mvwaddch(input->GetWindow(), i_ny, i_nx, ' ');
	wmove(input->GetWindow(), i_ny, i_nx);

	input->Update();
	return 1;
}

void ChatRoom::SetInputCursor(int y, int x)
{
	input->SetCursor(y, x);
	i_ny = y;
	i_nx = x;
}