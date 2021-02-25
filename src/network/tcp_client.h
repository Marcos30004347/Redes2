#ifndef NETWORKCP_CLIENT_H
#define NETWORKCP_CLIENT_H

/**
 * @brief TCP Client data structure.
 */
struct tcp_client;

/**
 * @brief The client receive message handler
 */
typedef void(*client_handler)(struct tcp_client*, char*);

/**
 * @brief Send a message to the server.
 */
void tcp_client_send(struct tcp_client* client, void* message, int len);

/**
 * @brief Allocates and initialize a tcp_client structure.
 */
struct tcp_client* tcp_client_create(const char* url, int port);

/**
 * @brief Destroys and dealocates a tcp_client structure.
 */
void tcp_client_destroy(struct tcp_client* client);

/**
 * @brief Disconnect a tcp_client from the server.
 */
void tcp_client_disconnect(struct tcp_client* client);

/**
 * @brief Try to receive a message from the server
 */
int tcp_client_receive(struct tcp_client* client, void* message, int length);

void tcp_client_set_timeout(struct tcp_client* client, unsigned long ms);

void tcp_client_remove_timeout(struct tcp_client* client);


#endif