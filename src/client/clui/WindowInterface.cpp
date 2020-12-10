#include "WindowInterface.hpp"

WindowInterface::WindowInterface(int num_y, int num_x, int by,
	int bx, char ch)
	: ny(num_y), nx(num_x), beg_y(by), beg_x(bx), ch_line(ch)
{
	w = newwin(ny, nx, beg_y, beg_x);
	box(w, ch_line, ch_line);
	Update();
}
WindowInterface::~WindowInterface()
{
	Clear(true);
	Update();
	delwin(w);
}

void WindowInterface::SetCursor(int y, int x)
{
	wmove(w, y, x);
}

void WindowInterface::Clear(bool full)
{
	werase(this->GetWindow());
	if(!full)
		box(this->GetWindow(), ch_line, ch_line);
}

void WindowInterface::Update()
{
	wrefresh(w);
}