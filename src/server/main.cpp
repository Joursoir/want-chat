#include <stdio.h>

#include "ChatServer.hpp"
#include "database.hpp"
#include "../config.hpp"

int main(int argc, char *argv[])
{
	/*	  Event-driven programming	  */
	/*
		1) selection event
		2) event handling 
	*/
	DatabaseManager *db = DatabaseManager::Connect(DATABASE_HOST, DATABASE_USER,
		DATABASE_PASSWD, DATABASE_DB, DATABASE_PORT);
	if(!db) {
		perror("database");
		return 1;
	}

	EventSelector *selector = new EventSelector;
	ChatServer *serv = ChatServer::Start(selector, db, SERVER_PORT);
	if(!serv) {
		perror("server");
		return 1;
	}
	selector->Run();
	return 0;
}