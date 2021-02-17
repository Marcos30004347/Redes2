#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "network/tcp_server.h"
#include "network/udp_server.h"
#include "network/signals.h"

#include "signals.h"

#define PORT_RANGE_START 1400
#define PORT_RANGE_END 65535

typedef struct {
    int ports[PORT_RANGE_END - PORT_RANGE_START];
    int i;
    int j;
} available_ports;

void init_available_ports(available_ports* buffer)
{
    buffer->i = 0;
    buffer->j = 1;

    for(int i=0; i<PORT_RANGE_END - PORT_RANGE_START; i++)
        buffer->ports[i] = i + PORT_RANGE_START;
}

int alloc_port(available_ports* buffer)
{
    if(buffer->j == buffer->i) return -1;

    int port = buffer->ports[buffer->j];
    buffer->ports[buffer->j] = -1;
    buffer->j = (buffer->j+1)%PORT_RANGE_END - PORT_RANGE_START;

    return port;
}

void free_port(available_ports* buffer, int port)
{
    buffer->ports[buffer->i] = port;
    buffer->i = (buffer->i+1)%PORT_RANGE_END - PORT_RANGE_START;
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
    printf("[UDP]: port='%i'\n", udp_port);

    udp_server_t_create(&udp_server, 0);
    udp_server_t_bind_to_port(udp_server, udp_port);

    // Build the connection responses
    char connection_response[6];

    // memset(connection_response, MESSAGE_CONNECTION, sizeof(short));
    // memset(&connection_response[2], udp_port, sizeof(int));
    *((short*)&connection_response[0])  = MESSAGE_CONNECTION;
    *((int*)&connection_response[2])    = udp_port;

    write(connection, connection_response, 6);

    char file_info[25];

    receive(connection, file_info, 25);

    short type      = *((short*)&file_info[0]);
    
    if(type != 3)
    {
        printf("Invalid response '%i' from server!\n", type);
        exit(-1);
    }

    char* arquivo    = ((char*)&file_info[2]);
    long file_size       = *((long*)&file_info[17]);
    
    printf("[FILE INFO]: arquivo='%s'\n", arquivo);
    printf("[FILE INFO]: file size='%li'\n", file_size);

    char* filepath = (char*)malloc(strlen(arquivo) + strlen("database/"));
    sprintf(filepath, "%s/%s", "database", arquivo);
    FILE* file = fopen(filepath, "w");
    free(filepath);
    // Alloc data structures: TODO
    long bytes_received = 0;

    short ok = 4;
    write(connection, &ok, sizeof(short));
    printf("[OK]: recebendo dados...\n");

    // RECEIVE DATA: TODO
    while(bytes_received != file_size)
    {
        char buffer[1008];
        int len;
        int received = udp_server_t_receice(udp_server, buffer, &len);
        
        short type = *((short*)&buffer[0]);

        if(type != 6)
        {
            printf("Invalid response '%i' from server!\n", type);
            exit(-1);
        }

        int sequence = *((int*)&buffer[2]);
        short payload_size = *((short*)&buffer[6]);
        bytes_received += payload_size;

        // printf("sequence: %i\n", sequence);
        // printf("payload_size: %i\n", payload_size);

        char string[payload_size];
        memcpy(string, &buffer[8], payload_size);

        fwrite(string, sizeof(char), payload_size, file);

        printf("already received: %li\n", bytes_received);
        printf("payload size: %i\n", payload_size);
        
        char ack[6];
    
        *((short*)&ack) = 7;
        *((int*)&ack[2]) = sequence;

        write(connection, ack, 6);
    }
    short fim = 5;
    write(connection, &fim, sizeof(short));
    printf("[FIM]: dados recebidos!\n");

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

    if(type == MESSAGE_HELLO) return hello(conn, receive, write);

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