#ifndef NETWORK_TCP_SERVER_H
#define NETWORK_TCP_SERVER_H

/**
 * @brief The TCP Server structure.
 * 
 */
struct tcp_server_t;
typedef struct tcp_server_t tcp_server_t;

typedef struct reply_t reply_t;

struct tcp_connection_t;
typedef struct tcp_connection_t tcp_connection_t;

// /**
//  * @brief This is a type that represents a pointer
//  * to a function that will handle a payload.
//  */
// typedef void(*tcp_server_t_handler)(struct request_t req, struct reply_t rep);


typedef long (*write_fn)(tcp_connection_t*, void*, int);
typedef long (*receive_fn)(tcp_connection_t*, void*, int);

typedef int(*tcp_server_t_handler)(tcp_connection_t*, receive_fn, write_fn);

/**
 * @brief This function allocates and initialize a TCP Server.
 * 
 * @param server A pointer tho the tcp_server_t that should be initialized.
 * @param flags The flags that will define the behavioud of the server.
 * 
 * @flags:
 *   * TCP_SERVER_SYNC - tels the server that it should process
 *   incoming requests in a syncronized way.
 */
void tcp_server_t_create(struct tcp_server_t** server,  tcp_server_t_handler handler, int port);
/**
 * @brief Deallocate and destroys a tcp_server_t structure.
 * 
 * @param server The server that should be destroyed.
 */
void tcp_server_t_destroy(struct tcp_server_t* server);

/**
 * @brief Bind a initialized struct tcp_server_t to a port in the host machine
 * 
 * @param server The server structure.
 * @param port The port that the server should listen.
 */
void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port);

/**
 * @brief Stops a server.
 * 
 * @param server The server that should stop running.
 */
void tcp_server_t_terminate(struct tcp_server_t* server);

/**
 * @brief Start a server making it accept connection.
 * 
 * @param server The server that should start executing.
 */
void tcp_server_t_start(struct tcp_server_t* server);

/**
 * @brief Disconnect a client from the server.
 * 
 */
void tcp_server_t_disconnect_client(struct tcp_connection_t* server);


int tcp_connection_t_get_id(struct tcp_connection_t* connection);

#endif