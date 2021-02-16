#include "udp_client.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>

struct udp_client_t;

typedef void(*client_handler)(struct udp_client_t*, char*);

struct udp_client_t
{
    struct sockaddr_in server_adress;
    int client_fd;
	client_handler on_receive;
};


int udp_client_t_receive(struct udp_client_t* client, char* message, int length)
{
	recvfrom(client->client_fd, message, sizeof(char)*length, MSG_WAITALL, &client->server_adress,  
            sizeof(client->server_adress));
	return 1;
}

void udp_client_t_send(struct udp_client_t* client, char* message)
{
	if(strlen(message) > TCP_CLIENT_MAX_PAYLOAD_LENGTH)
		return;

    sendto(client->client_fd, (const char *)message, strlen(message), 
        MSG_CONFIRM, (const struct sockaddr *) &client->server_adress,  
            sizeof(client->server_adress)); 
}


void udp_client_t_create(struct udp_client_t** client, const char* url, int port)
{
    *client = (struct udp_client_t*)malloc(sizeof(struct udp_client_t));
    struct udp_client_t* c = *client;

    c->client_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (c->client_fd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	}

    c->server_adress.sin_family = AF_INET; 
	c->server_adress.sin_addr.s_addr = inet_addr(url); 
	c->server_adress.sin_port = htons(port); 
}

void udp_client_t_destroy(struct udp_client_t* client)
{
	free(client);
}

void udp_client_t_disconnect(struct udp_client_t* client)
{
	close(client->client_fd);
}