#ifndef NETWORK_TCP_CLIENT_H
#define NETWORK_TCP_CLIENT_H

/**
 * @brief TCP Client data structure.
 */
struct tcp_client_t;

/**
 * @brief The client receive message handler
 */
typedef void(*client_handler)(struct tcp_client_t*, char*);

/**
 * @brief Send a message to the server.
 */
void tcp_client_t_send(struct tcp_client_t* client, void* message, int len);

/**
 * @brief Allocates and initialize a tcp_client_t structure.
 */
void tcp_client_t_create(struct tcp_client_t** client, const char* url, int port);

/**
 * @brief Destroys and dealocates a tcp_client_t structure.
 */
void tcp_client_t_destroy(struct tcp_client_t* client);

/**
 * @brief Disconnect a tcp_client_t from the server.
 */
void tcp_client_t_disconnect(struct tcp_client_t* client);

/**
 * @brief Try to receive a message from the server
 */
int tcp_client_t_receive(struct tcp_client_t* client, void* message, int length);

#endif