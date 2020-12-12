#ifndef WC_GUI_CLIENT_H
#define WC_GUI_CLIENT_H

#include "../../const_vars.hpp"
#include "../ClientBase.hpp"
#include "OO_FLTK.hpp"

class Client : public ClientBase {
	ChatBaseOutput **chat;
	ChatBaseOutput **players;
	ChatBaseOutput **tips;
public:
	Client(const char* ip, int port, ChatBaseOutput **i_chat,
		ChatBaseOutput **i_players, ChatBaseOutput **i_tips) 
		: ClientBase(ip, port), chat(i_chat), players(i_players),
		tips(i_tips) { }
	~Client() {}

	virtual void HandleActions() {}
	virtual void AddMessage(const char *msg, const char spec_char);

private:
	void UpdatePlayerList(const char *list);
	void SetGeneralOnline(const char *online);
	void SetRoomOnline(const char *online);
	void UpdateMessagesInChat(const char *msg, const char spec_char);
};

#endif