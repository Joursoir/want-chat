#ifndef WC_DATABASE_H
#define WC_DATABASE_H

#include <mysql.h>

class DatabaseManager;

class DB_ROW {
	 MYSQL_ROW row;
	 const unsigned int fields;
public:
	 DB_ROW(MYSQL_ROW *r, const unsigned int flds) :
	 	row(*r), fields(flds) {}
	 ~DB_ROW();
	 char *operator[](const int index);

	 const unsigned int GetFieldCount() const { return fields; }
};

class AnswerDB {
	friend class DatabaseManager; // ?

	MYSQL_RES *result;
	DB_ROW *current_row;
	const unsigned int fields;
	unsigned long rows;

	AnswerDB(MYSQL_RES *res, unsigned int flds, unsigned long rws) :
		result(res), current_row(0), fields(flds), rows(rws) {}
public:
	~AnswerDB();

	DB_ROW *GetNextRow();
	const unsigned int GetFieldCount() const { return fields; }
	unsigned long GetRowCount() const { return rows; }
};

class DatabaseManager {
	MYSQL *connection;

	DatabaseManager(MYSQL *db) : connection(db) {}
public:
	~DatabaseManager();

	static DatabaseManager *Connect(const char *host, const char *user,
		const char *pass, const char *db_name, unsigned int port,
    	const char *unix_socket = 0, unsigned long flags = 0);

	AnswerDB *QuerySelect(const char *sql);
};

#endif

/*

надо как-то вызывать ~AnswerDB!!

Answer DB *ans = server->Query("SELECT password FROM users WHERE name = '%s' ONLY ONE", strlen(..))
DB_ROW *row = ans->GetNextRow();
if(row)
	name = row[0];
	password = row[1];


*/