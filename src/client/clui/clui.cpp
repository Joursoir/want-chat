#include <string.h>
#include <unistd.h>
#include <ncurses.h>

#include "../dimensions_client.hpp"
#include "clui.hpp"

WindowChat::~WindowChat()
{
	while(first) {
        message *tmp = first;
        first = first->prev;
        delete tmp;
    }
}

void WindowChat::AddMessage(const char *msg, const char spec_ch)
{
	message *recent_msg = new message;

	int len = strlen(msg);
	int lines = (len / CHAT_COLUMNS) + 1;

	strcpy(recent_msg->text, msg);
	recent_msg->num_lines = lines;
	recent_msg->spec_char = spec_ch;

	recent_msg->prev = first;
	first = recent_msg;

	ChatRedraw();
}

void WindowChat::ChatRedraw()
{
	if(!first)
		return;

	Clear(false);
	int available_lines = CHAT_LINES;
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
		if(m->spec_char == SYSTEM_CHAR) wattron(w, A_ITALIC);
		else wattron(w, A_BOLD);
		wmove(w, line-need_print+1, 1);

		char *tmp = new char[CHAT_COLUMNS];
		int str = CHAT_COLUMNS * (m->num_lines - need_print);
		memcpy(tmp, m->text + str, CHAT_COLUMNS);

		wprintw(w, tmp);
		if(m->spec_char == SYSTEM_CHAR) wattroff(w, A_ITALIC);
		else wattroff(w, A_BOLD);
		
		delete[] tmp;
		need_print--;
	}
}

////////////////////////////////////////////////////////////////////////

WindowPlayers::WindowPlayers(int num_y, int num_x, int by, int bx, char ch)
	: WindowInterface(num_y, num_x, by, bx, ch)
{
	mvwprintw(w, num_y-2, 1, "<- /prev  /next ->");
	Update();
}

void WindowPlayers::SetPlayersList(const char *list)
{
	// clear line:
	for(int j = 1; j < ny-2; j++) {
		wmove(w, j, 1);
		for(int i = 1; i < nx-1; i++)
			waddch(w, ' ');
	}
	
	int len_list = strlen(list);
	int p = 1;

	int start = 0;
	for(int i = 0; i < len_list; i++) {
        if(list[i] == ';')
        {
        	char *str = new char[max_name_len+1];
        	int size = i - start;
        	memcpy(str, list+start, size);
        	str[size] = '\0';

        	mvwprintw(w, p, 1, str);
			start = i+1;
			p++;
			delete[] str;
        }
    }
    Update();
}

////////////////////////////////////////////////////////////////////////

WindowTips::WindowTips(int num_y, int num_x, int by, int bx, char ch)
	: WindowInterface(num_y, num_x, by, bx, ch)
{
	mvwprintw(w, num_y-7, 1, "       Tips");
	mvwprintw(w, TIPS_LINE_GONLINE, 1, "Online: ");
	mvwprintw(w, TIPS_LINE_RONLINE, 1, "Online room: ");
	mvwprintw(w, num_y-4, 1, "");
	mvwprintw(w, num_y-3, 1, "ESC - exit");
	mvwprintw(w, num_y-2, 1, "ENTER - send msg");
	Update();
}

void WindowTips::SetGeneralOnline(const char *online)
{
	// clear line:
	wmove(w, TIPS_LINE_GONLINE, 1);
	for(int i = 1; i < nx-1; i++)
		waddch(w, ' ');

	// print online
	char *str = new char[nx];
	sprintf(str, "Online: %s", online);
	mvwprintw(w, TIPS_LINE_GONLINE, 1, str);
	delete[] str;
	Update();
}

void WindowTips::SetRoomOnline(const char *online)
{
	// clear line:
	wmove(w, TIPS_LINE_RONLINE, 1);
	for(int i = 1; i < nx-1; i++)
		waddch(w, ' ');

	// print online
	char *str = new char[nx];
	sprintf(str, "Online room: %s", online);
	mvwprintw(w, TIPS_LINE_RONLINE, 1, str);
	delete[] str;
	Update();
}


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