#ifndef NETWORK_TCP_SERVER_H
#define NETWORK_TCP_SERVER_H

struct tcp_server_t;
typedef struct tcp_server_t tcp_server_t;

typedef struct reply_t reply_t;

struct tcp_connection_t;
typedef struct tcp_connection_t tcp_connection_t;


typedef int(*tcp_server_t_handler)(tcp_connection_t*);

/**
 * @brief This function allocates and initialize a TCP Server.
 */
void tcp_server_t_create(struct tcp_server_t** server,  tcp_server_t_handler handler, int port);
/**
 * @brief Deallocate and destroys a tcp_server_t structure.
 */
void tcp_server_t_destroy(struct tcp_server_t* server);

/**
 * @brief Bind a initialized struct tcp_server_t to a port in the host machine
 */
void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port);

/**
 * @brief Stops a server.
 */
void tcp_server_t_terminate(struct tcp_server_t* server);

/**
 * @brief Start a server making it accept connection.
 */
void tcp_server_t_start(struct tcp_server_t* server);

/**
 * @brief Disconnect a client from the server.
 */
void tcp_server_t_disconnect_client(struct tcp_connection_t* server);

int tcp_connection_t_get_id(struct tcp_connection_t* connection);

long connection_read(tcp_connection_t* con, void* buffer, int len);

long connection_write(tcp_connection_t* con, void* buffer, int len);

#endif