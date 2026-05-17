#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

int db_connect();
void db_close();

int db_execute(const char *query);

MYSQL_RES* db_query(const char *query);

#endif