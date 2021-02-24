#include "tcp_server.h"

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
    struct tcp_server_t*            server;
    int                             connfd;
};

struct tcp_connection_t {
    tcp_server_t*   server;
    int             client_fd;
};

struct tcp_server_t {
    int                             server_fd;
    struct sockaddr_storage         address;
    struct connection_node_t*       connections;
    tcp_server_t_handler            handler;
};



long connection_read(tcp_connection_t* con, void* buffer, int len) 
{
    return read(con->client_fd, buffer, len);
}

long connection_write(tcp_connection_t* con, void* buffer, int len) 
{
    write(con->client_fd, buffer, len); 
}

void* tcp_server_t_client_handler(void* _data)
{
    struct thread_data* data = (struct thread_data*)(_data);
    tcp_connection_t connection;
    connection.client_fd    = data->connfd;
    connection.server       = data->server;

    data->server->handler(&connection);

    free(data);

    return NULL;
}

void tcp_server_t_hold_connection(struct tcp_server_t* server, struct connection_t connection) {
    struct thread_t* thread = NULL;

    struct thread_data* data =(struct thread_data*) malloc(sizeof(struct thread_data));

    data->connfd = connection.client_fd;
    data->server = server;

    struct connection_node_t* conn = (struct connection_node_t*)malloc(sizeof(struct connection_node_t));

    if(!data->server->connections)
    {
        data->server->connections = conn;
    }
    else
    {
        data->server->connections->next = conn;
        conn->prev = data->server->connections;
        data->server->connections = conn;
    }

    thread_t_create(&thread, tcp_server_t_client_handler, data);

    conn->thread = thread;
    conn->client = connection.client_fd;
}

void tcp_server_t_create(struct tcp_server_t** server,  tcp_server_t_handler handler, int port)
{
    int no = 0;
    int reuseaddr = 1;

    *server = (struct tcp_server_t*)malloc(sizeof(struct tcp_server_t));
    
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)&(*server)->address;

    (*server)->server_fd = socket(AF_INET6, SOCK_STREAM, 0);

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
    (*server)->handler = handler;


    if ((bind((*server)->server_fd, (struct sockaddr *)addr, sizeof(*addr)) != 0)) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
}

void tcp_server_t_destroy(struct tcp_server_t* server)
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

    free(server);
}

struct connection_t tcp_server_t_accept_connection(struct tcp_server_t* server)
{
    struct connection_t conn;
    socklen_t len = sizeof(conn.client_address); 
    conn.client_fd = accept(server->server_fd, (struct sockaddr*)&conn.client_address, &len); 
    return conn;
}

void tcp_server_t_start(struct tcp_server_t* server)
{
    if ((listen(server->server_fd, 10)) != 0) { 
        printf("[ERROR]: Nao foi possivel escutar\n"); 
        exit(0); 
    }

    printf("listening...\n");
    struct connection_t connection = tcp_server_t_accept_connection(server);
    printf("connection!\n");

    while(connection.client_fd != -1)
    {
        tcp_server_t_hold_connection(server, connection);
        connection = tcp_server_t_accept_connection(server);
    }
}

void tcp_server_t_terminate(struct tcp_server_t* server)
{
    struct connection_node_t * tmp = server->connections;

    close(server->server_fd);
}

void tcp_server_t_disconnect_client(struct tcp_connection_t* conn)
{
    close(conn->client_fd);
}

int tcp_connection_t_get_id(struct tcp_connection_t* connection) {
    return connection->client_fd + 2020;
}