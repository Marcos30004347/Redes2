#include "tcp_server.h"

#include "async.h"

#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

typedef struct connection_node {
    int               client;
    struct thread*    thread;
    
    struct connection_node*  next;
    struct connection_node*  prev;
} connection_node;

typedef struct connection {
    int                 client_fd;
    struct sockaddr_in  client_address;
} connection;

typedef struct thread_data {
    struct tcp_server*   server;
    int                  connfd;
} thread_data;

typedef struct tcp_connection {
    tcp_server* server;
    int         client_fd;
} tcp_connection;

typedef struct tcp_server {
    int                     port;
    int                     server_fd;
    struct sockaddr_storage address;
    connection_node*        connections;
    tcp_server_handler      handler;
} tcp_server;



long connection_read(tcp_connection* con, void* buffer, int len)  {
    return read(con->client_fd, buffer, len);
}

long connection_write(tcp_connection* con, void* buffer, int len)  {
   return write(con->client_fd, buffer, len); 
}

void* tcp_server_client_handler(void* _data) {
    thread_data* data = (thread_data*)_data;
    tcp_connection connection;
    connection.client_fd    = data->connfd;
    connection.server       = data->server;

    data->server->handler(&connection);

    free(data);

    return NULL;
}

void tcp_server_hold_connection(tcp_server* server, struct connection connection) {
    thread_data* data =(thread_data*) malloc(sizeof(thread_data));

    data->connfd = connection.client_fd;
    data->server = server;

    connection_node* conn = (connection_node*)malloc(sizeof(struct connection_node));

    if(!data->server->connections) {
        data->server->connections = conn;
    }
    else {
        data->server->connections->next = conn;
        conn->prev = data->server->connections;
        data->server->connections = conn;
    }

    thread* t = thread_create(tcp_server_client_handler, data);

    conn->thread = t;
    conn->client = connection.client_fd;
}

tcp_server* tcp_server_create(tcp_server_handler handler, int port) {
    int no = 0;
    int reuseaddr = 1;

    tcp_server* server = (tcp_server*)malloc(sizeof(tcp_server));
    
    server->port = port;
    
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&server->address;

    server->server_fd = socket(AF_INET6, SOCK_STREAM, 0);

    if (server->server_fd == -1) { 
        printf("[ERROR]: criacao do socket falhou!\n"); 
        exit(0); 
    }

    setsockopt(server->server_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)); 
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr,sizeof(reuseaddr)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    memset(addr, 0, sizeof(*addr));

    addr->sin6_family = AF_INET6;
    addr->sin6_port = htons(port);
    addr->sin6_addr = in6addr_any;

    server->connections = NULL;
    server->handler = handler;


    if (bind(server->server_fd, (struct sockaddr *)addr, sizeof(*addr)) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 

    return server;
}

void tcp_server_destroy(tcp_server* server) {
    connection_node * tmp;
    while (server->connections) {
        tmp = server->connections;

        server->connections = server->connections->prev;
        if(server->connections)
            server->connections->next = NULL;

        thread_destroy(tmp->thread);
        free(tmp);
    }

    free(server);
}

struct connection tcp_server_accept_connection(tcp_server* server) {
    connection conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

void tcp_server_start(tcp_server* server) {
    if (listen(server->server_fd, 10) != 0) { 
        printf("[ERROR]: Nao foi possivel escutar\n"); 
        exit(0); 
    }

    printf("Server Listening on port '%i'!\n", server->port);
    connection connection;

    do {
        connection = tcp_server_accept_connection(server);
        tcp_server_hold_connection(server, connection);
    } while(connection.client_fd != -1);
}


void tcp_server_disconnect_client(tcp_connection* conn) {
    close(conn->client_fd);
}

