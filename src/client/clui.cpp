#include <string.h>
#include <ncurses.h>

#include <unistd.h>
#include "clui.hpp"
#include "../const_vars.hpp"

#define CHAT_HEIGHT 20
#define CHAT_WIDTH 59
#define PLAYERS_WIDTH 20
#define PLAYERS_HEIGHT 20
#define INPUT_HEIGHT 4
#define INPUT_WIDTH 80

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
	int len = strlen(msg);
	if(len > CHAT_WIDTH-2 + CHAT_WIDTH-2) lines = 3;
	else if(len > CHAT_WIDTH-2) lines = 2;

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

	chat->Clear(false);
	int available_lines = CHAT_HEIGHT - 2;
	bool remove = false;
	message *tmp;

	message *last;
	for(message *m = first; m; m = m->prev) {
		if(available_lines-1 >= m->num_lines) {
			PrintMessage(available_lines, m);
			available_lines -= m->num_lines;
			last = m;
		}
		else {
			tmp = m;
			remove = true;
			break;
		}
	}

	if(remove) {
		last->prev = 0;
		while(tmp) {
			message *m = tmp;
			tmp = tmp->prev;
			delete m;
		}
	}

	SetInputCursor(i_ny, i_nx);
	chat->Update();
}

void ChatRoom::PrintMessage(int line, message *m)
{
	WINDOW *win = this->GetChatWin();
	int need_print = m->num_lines;
	int maxlen_oneline = CHAT_WIDTH-2;

	while(need_print != 0) {
		wmove(win, line-need_print+1, 1);

		char *tmp = new char[maxlen_oneline];
		int str = maxlen_oneline * (m->num_lines - need_print);
		memcpy(tmp, m->msg + str, maxlen_oneline);

		wprintw(win, tmp);
		need_print--;

		delete[] tmp;
	}
}

bool ChatRoom::AddCharToSendMsg(char ch)
{
	if(i_ny == 2 && i_nx == max_line_len/2-1)
		return 0;

	mvwaddch(input->GetWindow(), i_ny, i_nx, ch);
	i_nx++;
	if(i_nx >= max_line_len/2-1) {
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
		i_nx = max_line_len/2-1;
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