#include "OO_FLTK.hpp"
#include "../../const_vars.hpp"

BoxOutline::BoxOutline(int x, int y, int w, int h, const char *lb)
	: Fl_Box(x, y, w, h, lb)
{
	box(FL_FLAT_BOX);
	color(FL_WHITE);
}

ChatInput::ChatInput(int x, int y, int w, int h, const char *lb)
	: Fl_Input(x, y, w, h, lb)
{
	callback(CallbackFunction, 0);
	when(FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

	box(FL_FLAT_BOX);
	color(FL_BLACK);

	cursor_color(FL_WHITE);
	textfont(FL_COURIER);
	textsize(20);
	textcolor(FL_WHITE);

	maximum_size(max_usermsg_len);
}

void ChatInput::SendMessage(void *user)
{
	if(strlen(value()) < 1)
		return;

	printf("SendMessage\n");
	// send message to server
	value("");
	take_focus();
}

ChatBaseOutput::ChatBaseOutput(int x, int y, int w, int h, const char *lb)
	: Fl_Multiline_Output(x, y, w, h, lb)
{
	box(FL_FLAT_BOX);
	color(FL_BLACK);

	textfont(FL_COURIER);
	textsize(20);
	textcolor(FL_WHITE);
}