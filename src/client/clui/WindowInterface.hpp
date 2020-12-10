#ifndef WC_WINDOW_INTERFACE_H
#define WC_WINDOW_INTERFACE_H

#include <ncurses.h>

class WindowInterface {
protected:
	WINDOW *w;
	int ny, nx;
	int beg_y, beg_x;
	int ch_line;
public:
	WindowInterface(int num_y, int num_x, int by, int bx, char ch);
	~WindowInterface();

	WINDOW *GetWindow() { return w; }
	void SetCursor(int y, int x);
	void Clear(bool full);
	void Update();
};

#endif