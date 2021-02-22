#include "udp_server.h"

#include "async.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

struct connection_node_t {
    int                             client;
    struct thread_t*                thread;
    
    struct connection_node_t*       next;
    struct connection_node_t*       prev;
};

struct connection_t {
    int                             client_fd;
    struct sockaddr_in              client_address;
};

struct thread_data {
    struct udp_server_t*            server;
    int                             connfd;
};

struct udp_server_t {
    int                             server_fd;
    struct sockaddr_storage         address;
    struct connection_node_t*       connections;
};

int udp_server_t_create(struct udp_server_t** server, int port)
{
    // *server = (struct udp_server_t*)malloc(sizeof(struct udp_server_t));
    
    // struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&(*server)->address;
    // addr6->sin6_family = AF_INET6;
    // addr6->sin6_addr = in6addr_any;
    // addr6->sin6_port = htons(port);

    // (*server)->kill_message = NULL;
    // (*server)->server_fd = socket((*server)->address.ss_family, SOCK_DGRAM, 0);
    // (*server)->request_handler = NULL;
    // (*server)->connections = NULL;
    
    // int no = 0;     
    // setsockopt((*server)->server_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
    
    // if ((*server)->server_fd == -1) { 
    //     printf("[ERRORÇ: criacao do socket falhou!\n"); 
    //     exit(0); 
    // }

    // if ((bind((*server)->server_fd, (struct sockaddr*)&(*server)->address, sizeof((*server)->address))) != 0) { 
    //     printf("socket bind failed...\n"); 
    //     exit(0); 
    // }

    // getsockname((*server)->server_fd, (struct sockaddr *)(&(*server)->address), &udpaddrlen);


    int no = 0;
    int reuseaddr = 1;

    *server = (struct udp_server_t*)malloc(sizeof(struct udp_server_t));
    
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&(*server)->address;

    (*server)->server_fd = socket(AF_INET6, SOCK_DGRAM, 0);

    if ((*server)->server_fd == -1) { 
        printf("[ERROR]: criacao do socket falhou!\n"); 
        exit(0); 
    }

    setsockopt((*server)->server_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
    if (setsockopt((*server)->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr,sizeof(reuseaddr)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    memset(addr, 0, sizeof(*addr));

    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;

    (*server)->connections = NULL;

    if (bind((*server)->server_fd, (struct sockaddr *)addr, sizeof(*addr)) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 

    socklen_t udpaddrlen = sizeof((*server)->address);
    getsockname((*server)->server_fd, (struct sockaddr *)(&(*server)->address), &udpaddrlen);

    return ntohs(((struct sockaddr_in6 *)(&(*server)->address))->sin6_port);
}

void udp_server_t_destroy(struct udp_server_t* server)
{
    struct connection_node_t * tmp;
    while (server->connections)
    {
        tmp = server->connections;

        server->connections = server->connections->prev;
        if(server->connections)
            server->connections->next = NULL;

        thread_t_destroy(tmp->thread);
        free(tmp);
    }

    close(server->server_fd);
    free(server);
}

struct connection_t udp_server_t_accept_connection(struct udp_server_t* server)
{
    struct connection_t conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

long udp_server_t_receice(struct udp_server_t* server, void* buffer, int* len) 
{
    struct sockaddr_in cliaddr;
    printf("asdasdasd\n");
    long resp = recvfrom(server->server_fd, buffer, 1024, MSG_WAITALL, ( struct sockaddr *) &cliaddr, len); 
    printf("asdasdasd %i\n", *len);
    printf("asdasdasd %li\n", resp);
    return resp;
}


void udp_server_t_terminate(struct udp_server_t* server)
{
    close(server->server_fd);
}


void udp_send_message_to_client(int client, const char* message)
{
    struct sockaddr_in servaddr, cliaddr; 
    int len;
    sendto(client, message, strlen(message), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len); 
}

void udp_server_t_disconnect_client(struct udp_server_t* server, int client)
{
    // struct connection_node_t ** connections = &server->connections;
    // while ((*connections) && (*connections)->client!=client)
    //     (*connections) = (*connections)->next;
    
    // if((*connections) && (*connections)->client == client)
    // {
    //     if((*connections)->prev) (*connections)->prev->next = (*connections)->next;
    //     if((*connections)->next) (*connections)->next->prev = (*connections)->prev;

    //     if(server->kill_message)
    //         udp_send_message_to_client(client, server->kill_message);

    //     thread_t_destroy((*connections)->thread);
    //     free((*connections));
    //     *connections = NULL;
    // }
}

