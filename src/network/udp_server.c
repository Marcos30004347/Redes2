#include "udp_server.h"

#include "async.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

struct connection_node {
    int                             client;
    struct thread*                thread;
    
    struct connection_node*       next;
    struct connection_node*       prev;
};

struct connection {
    int                             client_fd;
    struct sockaddr_in              client_address;
};

struct thread_data {
    struct udp_server*            server;
    int                             connfd;
};

struct udp_server {
    int                             server_fd;
    struct sockaddr_storage         address;
    struct connection_node*       connections;
};

struct udp_server* udp_server_create(int port)
{
    int no = 0;
    int reuseaddr = 1;

    struct udp_server* server = (struct udp_server*)malloc(sizeof(struct udp_server));
    
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&server->address;

    server->server_fd = socket(AF_INET6, SOCK_DGRAM, 0);

    if (server->server_fd == -1) { 
        printf("[ERROR]: criacao do socket falhou!\n"); 
        exit(0); 
    }

    setsockopt(server->server_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr,sizeof(reuseaddr)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    memset(addr, 0, sizeof(*addr));

    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;

    server->connections = NULL;

    if (bind(server->server_fd, (struct sockaddr *)addr, sizeof(*addr)) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 

   return server;
}

int udp_server_get_port(struct udp_server* server) {
    socklen_t udpaddrlen = sizeof(server->address);
    getsockname(server->server_fd, (struct sockaddr *)(&server->address), &udpaddrlen);
    return ntohs(((struct sockaddr_in6 *)(&server->address))->sin6_port);
}


void udp_server_destroy(struct udp_server* server)
{
    struct connection_node * tmp;
    while (server->connections)
    {
        tmp = server->connections;

        server->connections = server->connections->prev;
        if(server->connections)
            server->connections->next = NULL;

        thread_destroy(tmp->thread);
        free(tmp);
    }

    close(server->server_fd);
    free(server);
}

struct connection udp_server_accept_connection(struct udp_server* server)
{
    struct connection conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

long udp_server_receice(struct udp_server* server, char* buff) 
{
    int len;
    struct sockaddr_in cliaddr;
    long resp = recvfrom(server->server_fd, buff, 1008, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
    return resp;
}


void udp_servererminate(struct udp_server* server)
{
    close(server->server_fd);
}


void udp_send_messageo_client(int client, const char* message)
{
    struct sockaddr_in servaddr, cliaddr; 
    int len;
    sendto(client, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
}

