#include "OO_FLTK.hpp"
#include "../../const_vars.hpp"
#include "Client.hpp"

BoxBackground::BoxBackground(int x, int y, int w, int h,
	const char *lb, Fl_Color clr)
	: Fl_Box(x, y, w, h, lb)
{
	box(FL_FLAT_BOX);
	color(clr);
}

ChatInput::ChatInput(int x, int y, int w, int h,
	const char *lb, Client *user)
	: Fl_Input(x, y, w, h, lb)
{
	callback(CallbackFunction, (void *)user);
	when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

	box(FL_FLAT_BOX);
	color(FL_BLACK);

	cursor_color(FL_WHITE);
	textfont(STD_FONT);
	textsize(20);
	textcolor(FL_WHITE);

	maximum_size(max_usermsg_len);
}

void ChatInput::SendMsg(void *user)
{
	if(strlen(value()) < 1)
		return;

	Client *cl = (Client *)user;
	if(cl != 0) { // send message to server
		const char *message = value();
		cl->SendMessage(message);
	}

	value("");
	take_focus();
}

ChatBaseOutput::ChatBaseOutput(int x, int y, int w, int h, const char *lb)
	: Fl_Output(x, y, w, h, lb)
{
	box(FL_FLAT_BOX);
	color(FL_BLACK);

	textfont(STD_FONT);
	textsize(20);
	textcolor(FL_WHITE);
	value("");
}