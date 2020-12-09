#ifndef WC_OOFLTK_H
#define WC_OOFLTK_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Output.H>

#define STD_FONT FL_COURIER

class Client;

class BoxBackground : public Fl_Box {
public:
	BoxBackground(int x, int y, int w, int h,
		const char *lb = 0, Fl_Color clr = FL_WHITE);
	~BoxBackground() {}
};

class ChatInput : public Fl_Input {
public:
	ChatInput(int x, int y, int w, int h, 
		const char *lb = 0, Client *user = 0);
	virtual ~ChatInput() {}
	virtual void SendMessage(void *user);
private:
	static void CallbackFunction(Fl_Widget *w, void *user)
		{ static_cast<ChatInput*>(w)->SendMessage(user); }
};

class ChatBaseOutput : public Fl_Output {
public:
	ChatBaseOutput(int x, int y, int w, int h, const char *lb = 0);
	~ChatBaseOutput() {}
};

#endif