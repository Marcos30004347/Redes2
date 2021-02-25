#ifndef NETWORKCP_SERVER_H
#define NETWORKCP_SERVER_H

struct tcp_server;
typedef struct tcp_server tcp_server;

typedef struct reply reply;

struct tcp_connection;
typedef struct tcp_connection tcp_connection;


typedef int(*tcp_server_handler)(tcp_connection*);

/**
 * @brief This function allocates and initialize a TCP Server.
 */
struct tcp_server* tcp_server_create(tcp_server_handler handler, int port);

/**
 * @brief Deallocate and destroys a tcp_server structure.
 */
void tcp_server_destroy(struct tcp_server* server);

/**
 * @brief Start a server making it accept connection.
 */
void tcp_server_start(struct tcp_server* server);

/**
 * @brief Disconnect a client from the server.
 */
void tcp_server_disconnect_client(struct tcp_connection* server);

long connection_read(tcp_connection* con, void* buffer, int len);

long connection_write(tcp_connection* con, void* buffer, int len);

#endif