#include <stdio.h>

#include "chat.hpp"

int main(int argc, char *argv[])
{
	/*	  Event-driven programming	  */
	/*
		1) selection event
		2) event handling 
	*/
	EventSelector *selector = new EventSelector;
	/*ChatServer *serv = ChatServer::Start(selector, port);
	if(!serv) {
		perror("server");
		return 1;
	}*/
	selector->Run();
	return 0;
}