#include <iostream>

#include <served/multiplexer.hpp>
#include <http_server.h>

// sudo apt-get install libmariadb3 libmariadb-dev
#include <mariadb/mysql.h>

// database using mariadb
struct connection_details
{
    const char *server, *user, *password, *database;
};

MYSQL *mysql_connection_setup(struct connection_details mysql_details)
{
    MYSQL *connection = mysql_init(NULL);

    if (!mysql_real_connect(connection, mysql_details.server, mysql_details.user,
                            mysql_details.password, mysql_details.database,
                            0, NULL, 0))
    {
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return connection;
}

MYSQL_RES *mysql_execute_query(MYSQL *connection, const char *sql_query)
{
    if (mysql_query(connection, sql_query))
    {
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        exit(1);
    }
    return mysql_use_result(connection);
}

int main(int argc, char *argv[])
{
    // initialize connection to database
    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;

    struct connection_details mysqlDetails;
    mysqlDetails.server = "localhost";
    mysqlDetails.user = "cpp_rest_user";
    mysqlDetails.password = "password123";
    mysqlDetails.database = "cpp_rest_db";

    con = mysql_connection_setup(mysqlDetails);
    res = mysql_execute_query(con, "select * from videos");

    std::cout << "Displaying database output: \n"
              << std::endl;

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        std::cout << row[0] << " | "
                  << row[1] << " | "
                  << row[2] << " | "
                  << row[3]
                  << std::endl
                  << std::endl;
    }
    mysql_free_result(res);
    mysql_close(con);

    // create http server and initialize endpoints
    served::multiplexer multiplexer;
    HttpServer http_server(multiplexer);

    http_server.InitialiseEndpoints();
    http_server.StartServer();

    return 0;
}