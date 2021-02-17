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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>
#include <poll.h>
struct udp_client_t;

typedef void(*client_handler)(struct udp_client_t*, char*);

struct udp_client_t
{
    struct sockaddr_in server_adress;
    int client_fd;
	client_handler on_receive;
};


int udp_client_t_receive(struct udp_client_t* client, void* message, int length)
{
	int len;
	recvfrom(client->client_fd, message, length, MSG_WAITALL, (struct sockaddr*)&client->server_adress, &len);
	return 1;
}



// int udp_client_t_receive_with_timeout(struct udp_client_t* client, void* message, int length)
// {
// 	int sd = client->client_fd;

// 	fd_set input;
// 	FD_ZERO(&input);
// 	FD_SET(sd, &input);

// 	struct timeval timeout = {2, 0};

// 	int n = select(sd + 1, &input, NULL, NULL, &timeout);
// 	printf("%i\n", n);

// 	if (n < 0 || !FD_ISSET(sd, &input)) return -1;
// 	else if(n == 0) 					return 0;
// 	printf("asdasdasd %i\n", n);

// 	int recvlen = recvfrom(sd, message, length, 0, NULL, NULL);
// 	// read(client->client_fd, message, sizeof(char)*length);
// 	return 1;

// 	// /* Receive UDP message */
// 	// printf("%i\n", recvlen);
// 	// if (recvlen >= 0) {
// 	// 	return 1;
// 	// }
// 	// else{
// 	// 	return 0;
// 	// }
// }


void udp_client_t_send(struct udp_client_t* client, void* message, int len)
{
    sendto(client->client_fd, message, len, 
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

	/* set receive UDP message timeout */
	int flags = fcntl(c->client_fd, F_GETFL, 0);
	fcntl(c->client_fd, F_SETFL , flags | O_NONBLOCK);

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