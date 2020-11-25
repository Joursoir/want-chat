#include <stdio.h>

#include "chat.hpp"

const int port = 3030;

int main(int argc, char *argv[])
{
	/*	  Event-driven programming	  */
	/*
		1) selection event
		2) event handling 
	*/
	EventSelector *selector = new EventSelector;
	Server *serv = Server::Start(selector, port);
	if(!serv) {
		perror("server");
		return 1;
	}
	selector->Run();
	return 0;
}