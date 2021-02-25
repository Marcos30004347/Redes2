#ifndef NETWORK_UDP_SERVER_H
#define NETWORK_UDP_SERVER_H

/**
 * @brief The TCP Server structure.
 * 
 */
struct udp_server;
typedef struct udp_server udp_server;

/**
 * @brief This is a type that represents a pointer
 * to a function that will handle a payload.
 */
typedef void(*udp_server_request_handler)();

/**
 * @brief This function allocates and initialize a TCP Server.
 * 
 * @param server A pointer tho the udp_server that should be initialized.
 * @param flags The flags that will define the behavioud of the server.
 * 
 * @flags:
 *   * udp_SERVER_SYNC - tels the server that it should process
 *   incoming requests in a syncronized way.
 */
struct udp_server* udp_server_create(int port);

int udp_server_get_port(struct udp_server* server);

/**
 * @brief Deallocate and destroys a udp_server structure.
 * 
 * @param server The server that should be destroyed.
 */
void udp_server_destroy(struct udp_server* server);


/**
 * @brief Stops a server.
 * 
 * @param server The server that should stop running.
 */
void udp_servererminate(struct udp_server* server);

// /**
//  * @brief Start a server making it accept connection.
//  * 
//  * @param server The server that should start executing.
//  */
// void udp_server_start(struct udp_server* server);

long udp_server_receice(struct udp_server* server, char* buffer);



/**
 * @brief Sends a message to a connected tcp client.
 * 
 * @param client The client that sould receive the message.
 * @param message The message that sould be send.
 */
void udp_send_messageo_client(int client, const char* message);



#endif