#include "tcp_server.h"

#include "signals.h"
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
    int                             flags;
    int                             server_fd;
    const char*                     kill_message;
    struct sockaddr_in              address;
    struct connection_node_t*       connections;
    struct mutex_t*                 req_lock;
    tcp_server_t_handler            handler;
};


void tcp_handler(void(*read)(char** message, int len))
{
    char type[2];
    read(&type, 2);
}


long receive_from_connection(tcp_connection_t con, char* buffer, int len) 
{
    return read(con.client_fd, buffer, len);
}

void write_to_connection(tcp_connection_t con, char* buffer, int len) 
{
    write(con.client_fd, buffer, len); 
}

void* tcp_server_t_client_handler(struct thread_data* data)
{
    tcp_connection_t connection;
    connection.client_fd    = data->connfd;
    connection.server       = data->server;

    while(data->server->handler(&connection, receive_from_connection, write_to_connection)){}

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

void tcp_server_t_create(struct tcp_server_t** server, tcp_server_t_handler handler, int flags)
{
    *server = (struct tcp_server_t*)malloc(sizeof(struct tcp_server_t));
    mutex_t_create(&(*server)->req_lock);

    (*server)->kill_message = NULL;
    (*server)->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    (*server)->flags = flags;
    (*server)->connections = NULL;
    (*server)->handler = handler;

    if ((*server)->server_fd == -1) { 
        printf("[ERRORÇ: criacao do socket falhou!\n"); 
        mutex_t_unlock((*server)->req_lock);
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

    mutex_t_destroy(server->req_lock);
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
    if ((listen(server->server_fd, 5)) != 0) { 
        printf("[ERROR]: Nao foi possivel escutar\n"); 
        exit(0); 
    }

    struct connection_t connection = tcp_server_t_accept_connection(server);

    while(connection.client_fd != -1)
    {
        tcp_server_t_hold_connection(server, connection);
        connection = tcp_server_t_accept_connection(server);
    }
}

void tcp_server_t_terminate(struct tcp_server_t* server)
{
    struct connection_node_t * tmp = server->connections;

    while(tmp)
    {
        if(server->kill_message)
            send_message_to_client(tmp->client, server->kill_message);
        tmp = tmp->prev;
    }

    close(server->server_fd);
}


void tcp_server_t_bind_to_port(struct tcp_server_t* server, int port)
{
    server->address.sin_family = AF_INET; 
    server->address.sin_addr.s_addr = htonl(INADDR_ANY); 
    server->address.sin_port = htons(port); 

    if ((bind(server->server_fd, (struct sockaddr*)&server->address, sizeof(server->address))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
}


void send_message_to_client(int client, const char* message)
{
    write(client, message, strlen(message)); 
}

void tcp_server_t_disconnect_client(struct tcp_server_t* server, int client)
{
    struct connection_node_t ** connections = &server->connections;
    while ((*connections) && (*connections)->client!=client)
        (*connections) = (*connections)->next;
    
    if((*connections) && (*connections)->client == client)
    {
        if((*connections)->prev) (*connections)->prev->next = (*connections)->next;
        if((*connections)->next) (*connections)->next->prev = (*connections)->prev;

        if(server->kill_message)
            send_message_to_client(client, server->kill_message);

        thread_t_destroy((*connections)->thread);
        free((*connections));
        *connections = NULL;
    }
}
