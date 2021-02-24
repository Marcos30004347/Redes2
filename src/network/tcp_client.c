#include "utils.h"
#include "tcp_client.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef void(*client_handler)(struct tcp_client_t*, char*);

struct tcp_client_t
{
    int client_fd;
	client_handler on_receive;
};


int addrparse(const char *addrstr, int port, struct sockaddr_storage *storage) {
    if (addrstr == NULL) {
        return -1;
    }

    return -1;
}


int tcp_client_t_receive(struct tcp_client_t* client, void* message, int length)
{
	int sd = client->client_fd;

	fd_set input;
	FD_ZERO(&input);
	FD_SET(sd, &input);

	struct timeval timeout = {2, 0};

	int n = select(sd + 1, &input, NULL, NULL, &timeout);

	if(n == 0) return 0;

	if (n < 0 || !FD_ISSET(sd, &input)) {
		return -1;
	}

	read(client->client_fd, message, sizeof(char)*length);
	return 1;
}

void tcp_client_t_send(struct tcp_client_t* client, void* message, int len)
{
	write(client->client_fd, message, len); 
}


void tcp_client_t_create(struct tcp_client_t** client, const char* url, int port)
{
    *client = (struct tcp_client_t*)malloc(sizeof(struct tcp_client_t));
    struct tcp_client_t* c = *client;

    printf("url: %s\n", url);
	// struct in6_addr serveraddr;
	// struct sockaddr_storage addr;

    // if (inet_pton(AF_INET, url, (struct in_addr *)&addr)) {
	// 	c->client_fd = socket(AF_INET, SOCK_STREAM, 0);
	// 	if (c->client_fd == -1) { 
	// 		printf("socket creation failed...\n"); 
	// 		exit(0); 
	// 	}
	// 	struct sockaddr_in * _addr = (struct sockaddr_in *)&(addr);
	// 	_addr->sin_family = AF_INET; 
	// 	_addr->sin_addr.s_addr = inet_addr(url); 
	// 	_addr->sin_port = htons(port); 

	// 	int conn = connect(c->client_fd, (struct sockaddr *)_addr, sizeof(*_addr));
	// 	if (conn != 0){
	// 		printf("connection with the server failed with %i...\n", conn); 
	// 		exit(0); 
	// 	}

	// 	int flags = fcntl(c->client_fd, F_GETFL);
	// 	fcntl(c->client_fd, F_SETFL ,flags | O_NONBLOCK);
	// } else {
		// if (inet_pton(AF_INET6, url, (struct in6_addr *)&(c->server_adress.sin_addr))) {
		printf("IPv6\n");

		struct in6_addr serveraddr;
		struct addrinfo hints, *res=NULL;
		
		memset(&hints, 0x00, sizeof(hints));
		
		hints.ai_flags    = AI_NUMERICSERV;
		hints.ai_family   = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		int rc = inet_pton(AF_INET, url, &serveraddr);
	
		if (rc == 1)    /* valid IPv4 text address? */
		{
			hints.ai_family = AF_INET;
			hints.ai_flags |= AI_NUMERICHOST;
		}
		else
		{
			rc = inet_pton(AF_INET6, url, &serveraddr);
			if (rc == 1) /* valid IPv6 text address? */
			{

				hints.ai_family = AF_INET6;
				hints.ai_flags |= AI_NUMERICHOST;
			}
		}
	
		char sport[10];
		itoa(port, sport, 10);

		rc = getaddrinfo(url, sport, &hints, &res);
		if (rc != 0)
		{
			printf("Host not found --> %s\n", gai_strerror(rc));
			if (rc == EAI_SYSTEM)
				perror("getaddrinfo() failed");
		}

		c->client_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		if (c->client_fd == -1) { 
			printf("socket creation failed...\n"); 
			exit(0); 
		}

		int conn = connect(c->client_fd, res->ai_addr, res->ai_addrlen);

		if (conn != 0){
			printf("connection with the server failed with %i...\n", conn); 
			exit(0); 
		}

		// int flags = fcntl(c->client_fd, F_GETFL);
		// fcntl(c->client_fd, F_SETFL ,flags | O_NONBLOCK);
	// }

}

void tcp_client_t_destroy(struct tcp_client_t* client)
{
	free(client);
}

void tcp_client_t_disconnect(struct tcp_client_t* client)
{
	close(client->client_fd);
}