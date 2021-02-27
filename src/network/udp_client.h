#ifndef NETWORK_UDP_CLIENT_H
#define NETWORK_UDP_CLIENT_H



struct udp_client;

typedef struct udp_client udp_client;

void udp_client_send(udp_client* client, void* message, int len);


udp_client* udp_client_create(const char* url, int port);

/**
 * @brief Destroys and dealocates a udp_client structure.
 */
void udp_client_destroy(udp_client* client);

/**
 * @brief Disconnect a udp_client from the server.
 */
void udp_client_disconnect(udp_client* client);


#endif