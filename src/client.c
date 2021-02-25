#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "network/tcp_client.h"
#include "network/udp_client.h"

#include "signals.h"
#include "sliding-window/buffer.h"
#include "sliding-window/sending_window.h"


void verify_server_message_code(short* buff, short response) {
    short value = *buff;
    if(value != response)
    {
        printf(
            "Invalid response '%i' from server, should be '%i'!\n",
            value,
            response
        );
        exit(-1);
    }
}


int main(int argc, char *argv[]) {
    if(argc < 3) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    const char* url = argv[1];
    int port = atoi(argv[2]);
    
    char* arquivo = argv[3];

    char* archive_name = malloc(strlen(arquivo) + 1);
    memcpy(archive_name, arquivo, strlen(arquivo) + 1);

    if(strlen(archive_name) > 15)
    {
        printf("[Error]: O nome do arquivo tem mais de 15 caracteres!\n");
        exit(-1);
    }

    const char s[2] = ".";

    char *token, *suffix;
    
    token = strtok(archive_name, s);

    int i = 0;

    while( token != NULL ) {
        i++;
        suffix = token;
        token = strtok(NULL, s);
    }

    if(i > 2)
    {
        printf("[Error]: O nome do arquivo tem multiplos '.'!\n");
        exit(-1);
    }

    if(strlen(suffix) != 3)
    {
        printf("[Error]: O suffixo do arquivo tem menos de 3 caracteres!\n");
        exit(-1); 
    }

    free(archive_name);

    sending_window* window;
    sending_window_create(&window, arquivo);

    struct tcp_client* client = tcp_client_create(url, port);

    short hello_msg = 1;
    short file_msg  = 3;
    short data_msg  = 6;
    short ack_msg   = 6;

    buffer* hello_buff  = buffer_create(2);
    buffer* ack_buff    = buffer_create(6);
    buffer* conn_buff   = buffer_create(6);
    buffer* file_buff   = buffer_create(25);
    buffer* ok_buff     = buffer_create(2);
    buffer* dados_buff  = buffer_create(1008);


    buffer_set(hello_buff, 0, &hello_msg, 2);

    tcp_client_send(client, buffer_get(hello_buff, 0), 2);

    tcp_client_receive(client, buffer_get(conn_buff, 0), 6);

    verify_server_message_code(buffer_get(conn_buff,0), 2);

    int udp_port = *(int*)buffer_get(conn_buff, 2);
    
    buffer_set(file_buff, 0, &file_msg, 2);
    buffer_set(file_buff, 2, arquivo, strlen(arquivo) + 1);
    buffer_set(file_buff, 17, &window->size, sizeof(long));

    tcp_client_send(client, buffer_get(file_buff, 0), 25);

    tcp_client_receive(client, buffer_get(ok_buff, 0), 2);

    verify_server_message_code(buffer_get(ok_buff,0), 4);

    struct udp_client* udp_client = NULL;

    udp_client_create(&udp_client, url, udp_port);

    tcp_client_set_timeout(client, 500);

    while(!sending_window_eof(window)) {
        for(int i=0; i < WINDOW_SIZE && window->head + i < window->frame_count; i++) {
            int payload_size = 1000;
            if(sending_window_has_sended(window, window->head + i)) continue;

            if(window->head + i == window->frame_count - 1) payload_size = window->size%FRAME_SIZE;;
            
            int sequence = window->head + i;

            buffer_set(dados_buff, 0, &data_msg, sizeof(short));
            buffer_set(dados_buff, 2, &sequence, sizeof(int));
            buffer_set(dados_buff, 6, &payload_size, sizeof(short));

            buffer_set(dados_buff, 8, sending_window_get_data_from_head(window, window->head + i), payload_size);
            
            udp_client_send(udp_client, buffer_get(dados_buff,0), 8+payload_size);
        }
    
        int frames = 0;

        while(
            !sending_window_eof(window)
            && frames < WINDOW_SIZE - 1
            && tcp_client_receive(client, buffer_get(ack_buff,0), 6)
        ) {
            frames++;
            verify_server_message_code(buffer_get(ack_buff,0), 7);
    
            unsigned sequence = *(unsigned*)buffer_get(ack_buff, 2);
            sending_window_ack_frame(window, sequence);
        }
    }

    tcp_client_remove_timeout(client);

    sending_window_destroy(window);

    short fim;

    tcp_client_receive(client, &fim, 2);

    if(fim != 5)
    {
        printf("Invalid response '%i' from server!\n", fim);
        exit(-1);
    }

    return 0;
}