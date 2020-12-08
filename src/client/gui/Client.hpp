#ifndef WC_GUI_CLIENT_H
#define WC_GUI_CLIENT_H

#include "../ClientBase.hpp"
#include "OO_FLTK.hpp"

class Client : public ClientBase {
	ChatBaseOutput *chat;
public:
	Client(const char* ip, int port, ChatBaseOutput *cb_out) 
		: ClientBase(ip, port), chat(cb_out) {}
	~Client() {}

	virtual void HandleActions() {}
	virtual void ShowMessage(const char *msg);
};

#endif