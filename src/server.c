#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/tcp_server.h"
#include "network/udp_server.h"

#include "signals.h"

#include "sliding-window/buffer.h"
#include "sliding-window/sliding_window.h"

void verify_message_code(buffer* buff, short response) {
    if(*(short*)buffer_get(buff, 0) != response)
    {
        printf(
            "Invalid response '%i' from client, should be '%i'!\n",
            *(short*)buffer_get(buff, 0),
            response
        );
        exit(-1);
    }
}

int hello(tcp_connection_t* connection)
{
    sliding_window* window      = NULL;
    udp_server_t* udp_server    = NULL;
    
    short conn_resp = 2;
    short ok_resp = 4;
    short end_resp = 5;

    int udp_port = udp_server_t_create(&udp_server, 0);

    buffer* conn_buff   = buffer_create(6);
    buffer* file_buff   = buffer_create(25);
    buffer* ok_buff     = buffer_create(2);
    buffer* end_buff    = buffer_create(2);
    buffer* win_buff    = buffer_create(2024);

    buffer_set(conn_buff, 0, &conn_resp, sizeof(short));
    buffer_set(conn_buff, 2, &udp_port, sizeof(int));

    connection_write(connection, conn_buff, 6);

    connection_read(connection, file_buff, 25);

    verify_message_code(file_buff, 3);

    char* arquivo       = ((char*)buffer_get(file_buff, 2));
    long file_size      = *((long*)buffer_get(file_buff, 17));
    long frame_size     = 1000;
    long frame_count    = (long)ceil(file_size/(float)frame_size);

    char* filepath = (char*)malloc(strlen(arquivo) + strlen("database/"));

    sprintf(filepath, "%s/%s", "output", arquivo);
    sliding_window_create(&window, filepath, frame_count);

    free(filepath);

    buffer_set(ok_buff, 0, &ok_resp, sizeof(short));
    connection_write(connection, ok_buff, sizeof(short));

    while(!sliding_window_eof(window)) {
        int received = udp_server_t_receice(udp_server, win_buff);
    
        short type = *((short*)&win_buff->buffer[0]);

        verify_message_code(win_buff, 6);

        int sequence = *((int*)&win_buff->buffer[2]);
        short payload_size = *((short*)&win_buff->buffer[6]);

        char string[payload_size];
        sliding_window_ack_frame(window, sequence, &win_buff->buffer[8], payload_size);

        char ack[6];

        *((short*)&ack) = 7;
        *((int*)&ack[2]) = sequence;

        connection_write(connection, ack, 6);
    }


    buffer_set(end_buff, 0, &end_resp, sizeof(short));
    connection_write(connection, end_buff, sizeof(short));

    buffer_destroy(conn_buff);
    buffer_destroy(file_buff);
    buffer_destroy(ok_buff);
    buffer_destroy(win_buff);

    udp_server_t_destroy(udp_server);
    sliding_window_destroy(window);
    
    return 1;
}


int server_handler(tcp_connection_t* conn)
{
    short type;
    connection_read(conn, &type, sizeof(short));

    if(type == MESSAGE_HELLO) {
        hello(conn);
    }

    tcp_server_t_disconnect_client(conn);
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