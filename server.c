#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/tcp_server.h"
#include "network/udp_server.h"

#include "signals.h"
#include "sliding_window.h"


int hello(tcp_connection_t* connection, receive_fn receive, write_fn write)
{
    // This function is responsable for create the udp server that will
    // receive the client data.

    udp_server_t* udp_server = NULL;
    int udp_port = udp_server_t_create(&udp_server, 0);
    printf("[UDP]: port='%i'\n", udp_port);

    // Build the connection responses
    char connection_response[6];
    *((short*)&connection_response[0])  = MESSAGE_CONNECTION;
    *((int*)&connection_response[2])    = udp_port;
    write(connection, connection_response, 6);

    char file_info[25];

    receive(connection, file_info, 25);

    short type = *((short*)&file_info[0]);
    if(type != 3)
    {
        printf("Invalid response '%i' from server!\n", type);
        exit(-1);
    }
    char* arquivo    = ((char*)&file_info[2]);
    long file_size       = *((long*)&file_info[17]);

    long frame_size = 1000;
    long frame_count = (long)ceil(file_size/(float)frame_size);

    printf("[FILE INFO]: arquivo='%s'\n", arquivo);
    printf("[FILE INFO]: file size='%li'\n", file_size);

    sliding_window* window;

    char* filepath = (char*)malloc(strlen(arquivo) + strlen("database/"));
    sprintf(filepath, "%s/%s", "database", arquivo);
    printf("[FILE INFO]: output='%s'\n", filepath);
    sliding_window_create(&window, filepath, frame_count);
    free(filepath);

    short ok = 4;
    write(connection, &ok, sizeof(short));
    printf("[OK]: recebendo dados...\n");


    // RECEIVE DATA: TODO
    char buffer [2048];
    int len;

    while(!sliding_window_eof(window))
    {
    

        printf("================== experando\n");
        int received = udp_server_t_receice(udp_server, buffer, &len);
        printf("================== recebi\n");
    
        short type = *((short*)&buffer[0]);

        if(type != 6)
        {
            printf("Invalid response '%i' from client!\n", type);
            exit(-1);
        }

        int sequence = *((int*)&buffer[2]);
        short payload_size = *((short*)&buffer[6]);

        char string[payload_size];
        printf("A %i\n", payload_size);
        sliding_window_ack_frame(window, sequence, &buffer[8], payload_size);
        printf("B %i\n", payload_size);

        char ack[6];

        *((short*)&ack) = 7;
        *((int*)&ack[2]) = sequence;

        write(connection, ack, 6);
    }

    // free(buffer);

    sliding_window_destroy(window);

    short fim = 5;

    write(connection, &fim, sizeof(short));

    printf("[FIM]: dados recebidos!\n");

    udp_server_t_destroy(udp_server);
    return 1;
}


int server_handler(tcp_connection_t* conn, receive_fn receive, write_fn write)
{
    short type;
    receive(conn, &type, sizeof(short));
    printf("%i\n", type);
    printf("%p\n", conn);

    if(type == MESSAGE_HELLO) {
        hello(conn, receive, write);
    }
    else
        printf("Invalid code from client!\n");

    tcp_server_t_disconnect_client(conn);
    printf("TERMINOU\n");
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc < 2) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    tcp_server_t* server = NULL;
    tcp_server_t_create(&server, server_handler, port);
    tcp_server_t_start(server);
    tcp_server_t_destroy(server);

    return 0;
}