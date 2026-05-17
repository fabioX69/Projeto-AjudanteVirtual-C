#include "../include/db.h"
#include <stdio.h>
#include <stdlib.h>

static MYSQL *conn;

/**
 * @brief Estabelece a conexão síncrona com o banco de dados MySQL.
 * @return 1 se a conexão for bem-sucedida, 0 em caso de erro.
 */
int db_connect()
{
    conn = mysql_init(NULL);

    my_bool ssl = 0;
    // Configurações para desabilitar a imposição estrita de SSL
    mysql_options(conn, MYSQL_OPT_SSL_ENFORCE, &ssl);
    mysql_options(conn, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &ssl);

    // NOTA: Evitar credenciais hardcoded. Mover para variáveis de ambiente.
    if (!mysql_real_connect(conn,
                            "localhost",
                            "",
                            "",
                            "",
                            3306,
                            NULL,
                            0))
    {
        // Alerta: Saída em stdout fica oculta em execução de GUI padrão
        printf("Erro conexão: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

/**
 * @brief Encerra a conexão ativa do banco de dados e libera recursos associados.
 */
void db_close()
{
    if (conn != NULL) {
        mysql_close(conn);
    }
}

/**
 * @brief Executa comandos SQL de mutação de estado (INSERT, UPDATE, DELETE).
 * @param query String SQL contendo a instrução a ser executada.
 * @return 1 para sucesso, 0 para falhas na query.
 */
int db_execute(const char *query)
{
    if (mysql_query(conn, query))
    {
        printf("Erro query: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

/**
 * @brief Executa comandos de consulta (SELECT) e retorna o conjunto de resultados.
 * @param query String SQL contendo a instrução de busca.
 * @return Ponteiro para a estrutura MYSQL_RES ou NULL em caso de falha.
 */
MYSQL_RES *db_query(const char *query)
{
    if (mysql_query(conn, query))
    {
        printf("Erro query: %s\n", mysql_error(conn));
        return NULL;
    }

    // Armazena o resultado completo da query no lado do cliente
    return mysql_store_result(conn);
}