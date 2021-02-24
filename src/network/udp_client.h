#ifndef NETWORK_UDP_CLIENT_H
#define NETWORK_UDP_CLIENT_H


/**
 * @brief TCP Client data structure.
 */
struct udp_client_t;

/**
 * @brief Send a message to the server.
 */
void udp_client_t_send(struct udp_client_t* client, void* message, int len);

/**
 * @brief Allocates and initialize a udp_client_t structure.
 */
void udp_client_t_create(struct udp_client_t** client, const char* url, int port);

/**
 * @brief Destroys and dealocates a udp_client_t structure.
 */
void udp_client_t_destroy(struct udp_client_t* client);

/**
 * @brief Disconnect a udp_client_t from the server.
 */
void udp_client_t_disconnect(struct udp_client_t* client);

/**
 * @brief Try to receive a message from the server
 */
int udp_client_t_receive(struct udp_client_t* client, void* message, int length);

int udp_client_t_receive_with_timeout(struct udp_client_t* client, void* message, int length);


#endif