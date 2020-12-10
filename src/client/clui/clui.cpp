#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "clui.hpp"

WindowChat::~WindowChat()
{
	while(first) {
        message *tmp = first;
        first = first->prev;
        delete tmp;
    }
}

void WindowChat::AddMessage(const char *msg, int type)
{
	message *recent_msg = new message;

	int len = strlen(msg);
	int lines = (len / oneline_len) + 1;

	strcpy(recent_msg->text, msg);
	recent_msg->num_lines = lines;
	recent_msg->type = type;

	recent_msg->prev = first;
	first = recent_msg;

	ChatRedraw();
}

void WindowChat::ChatRedraw()
{
	if(!first)
		return;

	Clear(false);
	int available_lines = lines_in_chat;
	bool remove = false;
	message *tmp;

	message *last;
	for(message *m = first; m; m = m->prev) {
		if(available_lines >= m->num_lines) {
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

	//SetInputCursor(i_ny, i_nx);
	Update();
}

void WindowChat::PrintMessage(int line, message *m)
{
	int need_print = m->num_lines;
	while(need_print != 0) {
		if(m->type == system_msg) wattron(w, A_ITALIC);
		else wattron(w, A_BOLD);
		wmove(w, line-need_print+1, 1);

		char *tmp = new char[oneline_len];
		int str = oneline_len * (m->num_lines - need_print);
		memcpy(tmp, m->text + str, oneline_len);

		wprintw(w, tmp);
		if(m->type == system_msg) wattroff(w, A_ITALIC);
		else wattroff(w, A_BOLD);
		
		delete[] tmp;
		need_print--;
	}
}

////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////

WindowInput::WindowInput(int num_y, int num_x, int by, int bx, char ch)
	: WindowInterface(num_y, num_x, by, bx, ch), i_ny(1), i_nx(1)
{
	nodelay(w, true);
	keypad(w, true);
}

int WindowInput::GetChar()
{
	return wgetch(w);
}

bool WindowInput::AddCharToSendMsg(char ch)
{
	if(i_ny == 2 && i_nx == max_usermsg_len/2-1)
		return 0;

	mvwaddch(w, i_ny, i_nx, ch);
	i_nx++;
	if(i_nx >= max_usermsg_len/2-1) {
		if(i_ny == 1) {
			i_ny++;
			i_nx = 1;
		}
		else if(i_ny == 2) 
			i_nx--;
		wmove(w, i_ny, i_nx);
	}

	Update();
	return 1;
}

bool WindowInput::RemoveCharFromMsg()
{
	if(i_ny == 1 && i_nx == 1)
		return 0;

	i_nx--;
	if(i_nx < 1) {
		i_ny--;
		i_nx = max_usermsg_len/2-1;
	}
	mvwaddch(w, i_ny, i_nx, ' ');
	wmove(w, i_ny, i_nx);

	Update();
	return 1;
}

void WindowInput::SetPosCursor(int y, int x)
{
	SetCursor(y, x);
	i_ny = y;
	i_nx = x;
}