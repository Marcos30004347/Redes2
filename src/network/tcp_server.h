#ifndef NETWORKCP_SERVER_H
#define NETWORKCP_SERVER_H

struct tcp_server;

struct tcp_connection;

typedef struct tcp_server tcp_server;
typedef struct reply reply;
typedef struct tcp_connection tcp_connection;

typedef int(*tcp_server_handler)(tcp_connection*);

tcp_server* tcp_server_create(tcp_server_handler handler, int port);

void tcp_server_destroy(tcp_server* server);

void tcp_server_start(tcp_server* server);

void tcp_server_disconnect_client(tcp_connection* server);

long connection_read(tcp_connection* con, void* buffer, int len);

long connection_write(tcp_connection* con, void* buffer, int len);

#endif