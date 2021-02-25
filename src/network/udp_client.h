#ifndef NETWORK_UDP_CLIENT_H
#define NETWORK_UDP_CLIENT_H


/**
 * @brief TCP Client data structure.
 */
struct udp_client;

/**
 * @brief Send a message to the server.
 */
void udp_client_send(struct udp_client* client, void* message, int len);

/**
 * @brief Allocates and initialize a udp_client structure.
 */
void udp_client_create(struct udp_client** client, const char* url, int port);

/**
 * @brief Destroys and dealocates a udp_client structure.
 */
void udp_client_destroy(struct udp_client* client);

/**
 * @brief Disconnect a udp_client from the server.
 */
void udp_client_disconnect(struct udp_client* client);

/**
 * @brief Try to receive a message from the server
 */
int udp_client_receive(struct udp_client* client, void* message, int length);

int udp_client_receive_withimeout(struct udp_client* client, void* message, int length);


#endif