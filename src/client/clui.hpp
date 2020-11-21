// CLUI - Command Line User Interface

#ifndef COMMANDLINEUI_H
#define COMMANDLINEUI_H

#include <ncurses.h>

class Interface_wc {
	WINDOW *w;
	int ny, nx;
	int beg_y, beg_x;
public:
	Interface_wc(int num_y, int num_x, int by, int bx, char ch);

	WINDOW *GetWindow() { return w; }
	void Update() { wrefresh(w); }
	void Delete() { delwin(w); }

	void Hide();
};

class SelectionMenu : public Interface_wc {
	char const *title;
	const char**choises;
	int choises_number;
	int current_choice;

public:
	SelectionMenu(char const *i_title, const char**i_choises, int choises_num,
		int num_y, int num_x, int by, int bx, char ch);

	int Handling();
};

class ChatRoom {
	Interface_wc *chat;
	Interface_wc *players;
	Interface_wc *input;
	int i_nx, i_ny;
public:
	ChatRoom();
	~ChatRoom();

	// for chat:
	void PrintMessage(const char *msg);

	// for players:
	//void AddPlayer()

	// for input:
	bool AddCharToSendMsg(char ch);
	bool RemoveCharFromMsg();

	WINDOW *GetWin() { return input->GetWindow(); }
};

#endif