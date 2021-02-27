#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

#include "network/async.h"
#include "network/tcp_server.h"
#include "network/udp_server.h"

#include "signals.h"

#include "sliding-window/buffer.h"
#include "sliding-window/receiving_window.h"

void verify_client_message_code(short* buff, short response) {
    short value = *buff;
    if(value != response) {
        printf(
            "Invalid response '%i' from client, should be '%i'!\n",
            value,
            response
        );
        exit(-1);
    }
}

int handler(tcp_connection* connection) {
    short type;

    connection_read(connection, &type, sizeof(short));
    verify_client_message_code(&type, 1);

    mutex* mut = mutex_create();
    thread* threads[WINDOW_SIZE];
    udp_server* udp_server = udp_server_create(0);;

    short conn_resp = 2;
    short ok_resp   = 4;
    short end_resp  = 5;
    short ack_resp  = 7;

    int udp_port = udp_server_get_port(udp_server);

    // Intermediate buffers
    buffer* conn_buff   = buffer_create(6);
    buffer* file_buff   = buffer_create(25);
    buffer* ok_buff     = buffer_create(2);
    buffer* win_buff    = buffer_create(1008);
    buffer* end_buff    = buffer_create(2);
    buffer* ack_buff    = buffer_create(6);

    buffer_set(conn_buff, 0, &conn_resp, sizeof(short));
    buffer_set(conn_buff, 2, &udp_port, sizeof(int));

    // Send message to the client telling the UDP port
    connection_write(connection, buffer_get(conn_buff, 0), 6);

    // Read the file parameters
    connection_read(connection, buffer_get(file_buff, 0), 25);

    verify_client_message_code(buffer_get(file_buff, 0), 3);

    // Frame params
    char* file_name  = ((char*)buffer_get(file_buff, 2));
    long  file_size = *((long*)buffer_get(file_buff, 17));

    // Verifique se o diretório de output existe
    // Caso contrário crie-o
    struct stat st = {0};
    if (stat("./output", &st) == -1) {
        mkdir("./output", 0700);
    }

    // Start the sliding window data structures
    char* filepath = (char*)malloc(strlen("teste.txt") + strlen("database/"));
    sprintf(filepath, "%s/%s", "output", "teste.txt");
    receiving_window* window = receiving_window_create(filepath, (long)ceil(file_size/(float)FRAME_SIZE));

    // Send message to the client telling that it can start sendind frames
    buffer_set(ok_buff, 0, &ok_resp, sizeof(short));
    connection_write(connection, buffer_get(ok_buff, 0), sizeof(short));
    
    while(!receiving_window_eof(window)) {
        udp_server_receice(udp_server, buffer_get(win_buff, 0));

        // Verify if the received message start with a '6'
        verify_client_message_code(buffer_get(win_buff, 0), 6);

        // Get arrived 'sequence' and 'payload size'
        int sequence        = *(int*)buffer_get(win_buff, 2);
        short payload_size  = *(short*)buffer_get(win_buff, 6);
    
        // Save the received frame
        receiving_window_ack_frame(window, sequence, buffer_get(win_buff, 8), payload_size);

        // Send ACK
        buffer_set(ack_buff, 0, &ack_resp, sizeof(short));
        buffer_set(ack_buff, 2, &sequence, sizeof(int));
        connection_write(connection, buffer_get(ack_buff, 0), 6);
    }

    // Set and send the end message, telling that everything have ended as expected
    buffer_set(end_buff, 0, &end_resp, sizeof(short));
    connection_write(connection, buffer_get(end_buff, 0), sizeof(short));

    // Destroy allocated datastructures
    buffer_destroy(conn_buff);
    buffer_destroy(file_buff);
    buffer_destroy(ok_buff);
    buffer_destroy(win_buff);
    
    udp_server_destroy(udp_server);
    receiving_window_destroy(window);
    
    mutex_destroy(mut);
    free(filepath);

    tcp_server_disconnect_client(connection);    
    return 1;
}


int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    int port = atoi(argv[1]);

    tcp_server* server = tcp_server_create(handler, port);
    tcp_server_start(server);
    tcp_server_destroy(server);

    return 0;
}
