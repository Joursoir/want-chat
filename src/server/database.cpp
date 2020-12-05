#include "database.hpp"

DB_ROW::~DB_ROW()
{
	row = 0;
	/* mariadb documentation:
	Memory associated to MYSQL_ROW will be freed when calling
	mysql_free_result() function. */
}

char *DB_ROW::operator[](const int index)
{
 	return row[index];
}
//////////////////////////////////////////////////////////////

AnswerDB::~AnswerDB()
{
	if(current_row)
		delete current_row;
	
	mysql_free_result(result);
}

DB_ROW *AnswerDB::GetNextRow()
{
	if(current_row)
		delete current_row;

	MYSQL_ROW row = mysql_fetch_row(result);
	if(!row)
		return 0;

	current_row = new DB_ROW(&row, fields);
	return current_row;
}

//////////////////////////////////////////////////////////////

DatabaseManager::~DatabaseManager()
{
	mysql_close(connection);
}

DatabaseManager *DatabaseManager::Connect(const char *host, const char *user,
	const char *pass, const char *db_name, unsigned int port,
    const char *unix_socket, unsigned long flags)
{
	MYSQL *mysql = 0;
	mysql = mysql_init(mysql);

	mysql = mysql_real_connect(mysql, host, user, pass,
		db_name, port, unix_socket, flags);
	if(!mysql)
		return 0;

	return new DatabaseManager(mysql);
}

AnswerDB *DatabaseManager::QuerySelect(const char *sql)
{
	int state = mysql_query(connection, sql);
	if(state != 0)
		return 0;
		// #TODO: error msg

	MYSQL_RES *result = mysql_store_result(connection);
	if(!result) 
		return 0;

	unsigned int fields = mysql_num_fields(result);
	unsigned long rows = mysql_num_rows(result);
	return new AnswerDB(result, fields, rows);
}

int DatabaseManager::QueryInsert(const char *sql)
{
	return mysql_query(connection, sql);
}