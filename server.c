#include <stdlib.h>
#include<string.h>

#include "network/tcp_server.h"
#include "network/udp_server.h"
#include "network/signals.h"

#include "signals.h"

typedef struct {
    int ports[65535 - 1024];
    int i;
    int j;
} available_ports;

void init_available_ports(available_ports* buffer)
{
    buffer->i = 0;
    buffer->j = 1;

    for(int i=0; i<65535 - 1024; i++)
        buffer->ports[i] = i + 1024;
}

int alloc_port(available_ports* buffer)
{
    if(buffer->j == buffer->i) return -1;

    int port = buffer->ports[buffer->j];
    buffer->ports[buffer->j] = -1;
    buffer->j = (buffer->j+1)%65535 - 1024;

    return port;
}

void free_port(available_ports* buffer, int port)
{
    buffer->ports[buffer->i] = port;
    buffer->i = (buffer->i+1)%65535 - 1024;
}

available_ports ports;


int udp_server_handler(tcp_connection_t* connection, receive_fn receive, write_fn write)
{

}

int hello(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    // This function is responsable for create the udp server that will
    // receive the client data.
    int udp_port = alloc_port(&ports);
    if(udp_port == -1) 
    {
        // Error
    }

    udp_server_t* udp_server = NULL;

    udp_server_t_create(&udp_server, 0);
    udp_server_t_bind_to_port(udp_server, udp_port);
    udp_server_t_start(udp_server);

    // Build the connection responses
    char connection_response[6];

    short* type = (short*)&connection_response[0];
    *type       = CONNECTION;
    int* port   = (int*)&connection_response[2];
    *port       = udp_port;

    write(connection, connection_response, 6);
    return SERVER_SUCCESS;
}

int info_file(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    return SERVER_SUCCESS;
}

int ok(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    return SERVER_SUCCESS;
}

int fim(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    return SERVER_SUCCESS;
}

int ack(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    return SERVER_SUCCESS;
}

int server_handler(tcp_connection_t* conn, receive_fn receive, write_fn write)
{
    short type;
    receive(conn, &type, sizeof(short));

    switch (type)
    {
        case HELLO:         return hello(conn, receive, write);
        case CONNECTION:    return connection(conn, receive, write);
        case INFO_FILE:     return info_file(conn, receive, write);
        case OK:            return ok(conn, receive, write);
        case FIM:           return fim(conn, receive, write);
        case ACK:           return ack(conn, receive, write);
    }

    return SERVER_FAILURE;
}

int main(int argc, char *argv[]) {
    // LOG("Starting Publish/Subscribe Server!\n");

    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);
    init_available_ports(&ports);

    tcp_server_t* server = NULL;
    tcp_server_t_create(&server, server_handler, TCP_SERVER_NONE);
    tcp_server_t_bind_to_port(server, port);
    tcp_server_t_start(server);
    tcp_server_t_destroy(server);

    return 0;
}