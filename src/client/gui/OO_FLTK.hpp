#ifndef WC_OOFLTK_H
#define WC_OOFLTK_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Output.H>

class BoxOutline : public Fl_Box {
public:
	BoxOutline(int x, int y, int w, int h, const char *lb = 0);
	~BoxOutline() {}
};

class ChatInput : public Fl_Input {
public:
	ChatInput(int x, int y, int w, int h, const char *lb = 0);
	virtual ~ChatInput() {}
	virtual void SendMessage(void *user);
private:
	static void CallbackFunction(Fl_Widget *w, void *user)
		{ static_cast<ChatInput*>(w)->SendMessage(user); }
};

class ChatBaseOutput : public Fl_Multiline_Output {
public:
	ChatBaseOutput(int x, int y, int w, int h, const char *lb = 0);
	~ChatBaseOutput() {}
};

#endif