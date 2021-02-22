#ifndef NETWORK_UDP_SERVER_H
#define NETWORK_UDP_SERVER_H

/**
 * @brief The TCP Server structure.
 * 
 */
struct udp_server_t;
typedef struct udp_server_t udp_server_t;

/**
 * @brief This is a type that represents a pointer
 * to a function that will handle a payload.
 */
typedef void(*udp_server_t_request_handler)();

/**
 * @brief This function allocates and initialize a TCP Server.
 * 
 * @param server A pointer tho the udp_server_t that should be initialized.
 * @param flags The flags that will define the behavioud of the server.
 * 
 * @flags:
 *   * udp_SERVER_SYNC - tels the server that it should process
 *   incoming requests in a syncronized way.
 */
int udp_server_t_create(struct udp_server_t** server, int port);

/**
 * @brief Deallocate and destroys a udp_server_t structure.
 * 
 * @param server The server that should be destroyed.
 */
void udp_server_t_destroy(struct udp_server_t* server);


/**
 * @brief Stops a server.
 * 
 * @param server The server that should stop running.
 */
void udp_server_t_terminate(struct udp_server_t* server);

// /**
//  * @brief Start a server making it accept connection.
//  * 
//  * @param server The server that should start executing.
//  */
// void udp_server_t_start(struct udp_server_t* server);

long udp_server_t_receice(struct udp_server_t* server, void* buffer, int* len);



/**
 * @brief Sends a message to a connected tcp client.
 * 
 * @param client The client that sould receive the message.
 * @param message The message that sould be send.
 */
void udp_send_message_to_client(int client, const char* message);

/**
 * @brief Disconnect a client from the server.
 * 
 * @param server The server that is holding the connection.
 * @param client The id of the client that should be disconnected.
 */
void udp_server_t_disconnect_client(struct udp_server_t* server, int client);



#endif