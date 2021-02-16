#ifndef NETWORK_TCP_CLIENT_H
#define NETWORK_TCP_CLIENT_H

#ifndef TCP_CLIENT_MAX_PAYLOAD_LENGTH
#define TCP_CLIENT_MAX_PAYLOAD_LENGTH 1224
#endif


/**
 * @brief TCP Client data structure.
 * 
 */
struct udp_client_t;

/**
 * @brief The client receive message handler
 */
typedef void(*client_handler)(struct udp_client_t*, char*);

/**
 * @brief Send a message to the server.
 * 
 * @param client The client that should send the message.
 * @param message The message that sould be send.
 */
void udp_client_t_send(struct udp_client_t* client, char* message);

/**
 * @brief Allocates and initialize a udp_client_t structure.
 * 
 * @param client The client that sould be initialized.
 * @param url The url of the server that the client should connect.
 * @param port The port that the server is listening.
 */
void udp_client_t_create(struct udp_client_t** client, const char* url, int port);

/**
 * @brief Destroys and dealocates a udp_client_t structure.
 * 
 * @param client The tcp client that should be destroyed.
 */
void udp_client_t_destroy(struct udp_client_t* client);

/**
 * @brief Disconnect a udp_client_t from the server.
 * 
 * @param client The tcp client that should be desconnected.
 */
void udp_client_t_disconnect(struct udp_client_t* client);

/**
 * @brief Try to receive a message from the server
 * 
 * @param client The client that should receive the message.
 * @param message A pointer to a memory address that the message should be stored.
 * @param length The upper bound length of the message that will be stored.
 * @return 1 if a message was received and -1 otherwise.
 */
int udp_client_t_receive(struct udp_client_t* client, char* message, int length);

#endif