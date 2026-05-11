#include "../include/db.h"
#include <stdio.h>
#include <stdlib.h>

static MYSQL *conn;

int db_connect()
{
    conn = mysql_init(NULL);

    my_bool ssl = 0;
    mysql_options(conn, MYSQL_OPT_SSL_ENFORCE, &ssl);
    mysql_options(conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl);

    if (!mysql_real_connect(conn,
                            "localhost",
                            "root",
                            "",
                            "db_assistente_virtual",
                            3306,
                            NULL,
                            0))
    {
        printf("Erro conexão: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

void db_close()
{
    mysql_close(conn);
}

int db_execute(const char *query)
{
    if (mysql_query(conn, query))
    {
        printf("Erro query: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

MYSQL_RES *db_query(const char *query)
{
    if (mysql_query(conn, query))
    {
        printf("Erro query: %s\n", mysql_error(conn));
        return NULL;
    }

    return mysql_store_result(conn);
}