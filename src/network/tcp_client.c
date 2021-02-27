#include "utils.h"
#include "tcp_client.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef void(*client_handler)(struct tcp_client*, char*);

typedef struct tcp_client
{
	int blocking;
	unsigned long ms;
    int client_fd;
    int flags;
	client_handler on_receive;
} tcp_client;


int addrparse(const char *addrstr, int port, struct sockaddr_storage *storage) {
    if (addrstr == NULL) {
        return -1;
    }

    return -1;
}

int tcp_client_receive(tcp_client* client, void* message, int length)
{
	if(client->blocking){
		int sd = client->client_fd;

		fd_set input;
		FD_ZERO(&input);
		FD_SET(sd, &input);

		struct timeval timeout = {0, client->ms};

		int n = select(sd + 1, &input, NULL, NULL, &timeout);

		if(n == 0) return 0;

		if (n < 0 || !FD_ISSET(sd, &input)) {
			return -1;
		}
	}

	read(client->client_fd, message, length);
	return 1;
}

void tcp_client_send(struct tcp_client* client, void* message, int len)
{
	write(client->client_fd, message, len); 
}


struct tcp_client* tcp_client_create(const char* url, int port)
{
    struct tcp_client* c = (struct tcp_client*)malloc(sizeof(struct tcp_client));
	c->blocking = 0;
	c->ms = 0;

	struct in6_addr serveraddr;
	struct addrinfo hints, *res=NULL;
	
	memset(&hints, 0x00, sizeof(hints));
	
	hints.ai_flags    = AI_NUMERICSERV;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (inet_pton(AF_INET, url, &serveraddr) == 1) {
		hints.ai_family = AF_INET;
		hints.ai_flags |= AI_NUMERICHOST;
	} else {
		if (inet_pton(AF_INET6, url, &serveraddr) == 1) {
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
		}
	}

	char sport[10];
	itoa(port, sport, 10);

	int rc = getaddrinfo(url, sport, &hints, &res);
	if (rc != 0) {
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

	return c;
}

void tcp_client_destroy(struct tcp_client* client)
{
	free(client);
}

void tcp_client_disconnect(struct tcp_client* client)
{
	close(client->client_fd);
}

void tcp_client_set_timeout(struct tcp_client* client, unsigned long ms) {
	client->blocking = 1;
	client->ms = ms;

	int flags = fcntl(client->client_fd, F_GETFL, 0);
	client->flags = flags;

	int result = fcntl(client->client_fd, F_SETFL , flags | O_NONBLOCK);

	if(result < 0) {
		printf("Erro em tcp_client_set_timeout\n");
		exit(-1);
	}
}

void tcp_client_remove_timeout(struct tcp_client* client) {
	client->blocking = 0;

	int result = fcntl(client->client_fd, F_SETFL , client->flags);

	if(result < 0) {
		printf("Erro em tcp_client_remove_timeout\n");
		exit(-1);
	}
}