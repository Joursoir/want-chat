#ifndef WC_GUI_CLIENT_H
#define WC_GUI_CLIENT_H

#include "../../const_vars.hpp"
#include "../ClientBase.hpp"
#include "OO_FLTK.hpp"

class Client : public ClientBase {
	ChatBaseOutput **chat;
public:
	Client(const char* ip, int port, ChatBaseOutput **cb_out) 
		: ClientBase(ip, port), chat(cb_out)
	{
		for(int i = 0; i < lines_in_chat; i++)
			cb_out[i]->value("");
	}
	~Client() {}

	virtual void HandleActions() {}
	virtual void AddMessage(const char *msg, int type);
};

#endif