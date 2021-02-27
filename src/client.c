#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "network/tcp_client.h"
#include "network/udp_client.h"
#include "network/async.h"

#include "signals.h"
#include "sliding-window/buffer.h"
#include "sliding-window/sending_window.h"

typedef struct parameters {
    int port;
    const char* url;
    const char* arquivo;
} parameters;

void verify_server_message_code(short* buff, short response);

parameters get_parameters(int argc, char** argv);

typedef struct thread_data {
    tcp_client* tcp_client;
    udp_client* udp_client;
    sending_window* window;
    mutex* mut;
    long i;
    long head;
} thread_data;

thread_data* thread_data_create(tcp_client* t, udp_client* u, sending_window* w, mutex* m, long frame, long head) {
    thread_data* d = (thread_data*)malloc(sizeof(thread_data));
    d->tcp_client = t;
    d->udp_client = u;
    d->window = w;
    d->mut = m;
    d->i = frame;
    d->head = head;
    return d;
}

void thread_data_destroy(thread_data* data) {
    free(data);
}

void* wait_ack_for_frame(void* data);
void* send_frame(void* data);

int main(int argc, char *argv[]) {
    parameters params = get_parameters(argc, argv);
    
    thread* window_threads[WINDOW_SIZE];

    mutex* mut = mutex_create();

    sending_window* window = sending_window_create(params.arquivo);
    tcp_client* client = tcp_client_create(params.url, params.port);

    short hello_msg = 1;
    short file_msg  = 3;
    short data_msg  = 6;
    short ack_msg   = 7;

    buffer* hello_buff  = buffer_create(2);
    buffer* conn_buff   = buffer_create(6);
    buffer* file_buff   = buffer_create(25);
    buffer* ok_buff     = buffer_create(2);

    buffer_set(hello_buff, 0, &hello_msg, 2);

    tcp_client_send(client, buffer_get(hello_buff, 0), 2);

    tcp_client_receive(client, buffer_get(conn_buff, 0), 6);

    verify_server_message_code(buffer_get(conn_buff,0), 2);

    int udp_port = *(int*)buffer_get(conn_buff, 2);
    
    buffer_set(file_buff, 0, &file_msg, 2);
    buffer_set(file_buff, 2, (void*)params.arquivo, strlen(params.arquivo) + 1);
    buffer_set(file_buff, 17, &window->size, sizeof(long));

    tcp_client_send(client, buffer_get(file_buff, 0), 25);

    tcp_client_receive(client, buffer_get(ok_buff, 0), 2);

    verify_server_message_code(buffer_get(ok_buff,0), 4);

    udp_client* udp_client = udp_client_create(params.url, udp_port);

    tcp_client_set_timeout(client, 300);

    while(!sending_window_eof(window)) {
        // Get the current window head
        // This is the first frame in the window
        long head = window->head;

        // Send the frames in parallel
        for(int i=0; i < WINDOW_SIZE; i++) {
            thread_data* data = thread_data_create(client, udp_client, window, mut, i, head);
            window_threads[i] = thread_create(send_frame, data);
        }

        // Wait all messages to be send
        for(int i=0; i < WINDOW_SIZE; i++) {
            thread_join(window_threads[i]);
            thread_destroy(window_threads[i]);
        }

        // Wait for the ACKs
        for(int i=0; i < WINDOW_SIZE; i++) {
            thread_data* data = thread_data_create(client, udp_client, window, mut, i, head);
            window_threads[i] = thread_create(wait_ack_for_frame, (void*)data);
        }

        // Wait for all the ACKs or for the timeout
        for(int i=0; i < WINDOW_SIZE; i++) {
            thread_join(window_threads[i]);
            thread_destroy(window_threads[i]);
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

    tcp_client_destroy(client);
    udp_client_destroy(udp_client);

    mutex_destroy(mut);

    buffer_destroy(hello_buff);
    buffer_destroy(ok_buff);
    buffer_destroy(file_buff);
    buffer_destroy(conn_buff);

    return 0;
}

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

parameters get_parameters(int argc, char** argv) {
    parameters params;

    if(argc < 3) {
        printf("Argumentos insuficientes!\n");
        exit(-1);
    }

    const char* url = argv[1];
    int port = atoi(argv[2]);

    params.url = url;
    params.port = port;

    char* arquivo = argv[3];
    params.arquivo = arquivo;

    char* archive_name = malloc(strlen(arquivo) + 1);
    memcpy(archive_name, arquivo, strlen(arquivo) + 1);

    if(strlen(archive_name) > 15) {
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

    if(i > 2) {
        printf("[Error]: O nome do arquivo tem multiplos '.'!\n");
        exit(-1);
    }

    if(strlen(suffix) != 3) {
        printf("[Error]: O suffixo do arquivo tem menos de 3 caracteres!\n");
        exit(-1); 
    }

    free(archive_name);

    return params;
}

void* wait_ack_for_frame(void* _data){
    thread_data* data = (thread_data*)_data;

    if(data->head + data->i >= data->window->frame_count) {
        thread_data_destroy(data);
        return NULL;
    }

    buffer* ack_buff    = buffer_create(6);

    if(tcp_client_receive(data->tcp_client, buffer_get(ack_buff,0), 6)) {
        verify_server_message_code(buffer_get(ack_buff,0), 7);

        unsigned sequence = *(unsigned*)buffer_get(ack_buff, 2);

        // Make sure that no two threads can deal with de window at the same time
        mutex_lock(data->mut);
        sending_window_ack_frame(data->window, sequence);
        mutex_unlock(data->mut);

    }

    buffer_destroy(ack_buff);
    thread_data_destroy(data);

    return NULL;
}

void* send_frame(void* _data) {
    thread_data* data = (thread_data*)_data;

    if(data->head + data->i >= data->window->frame_count) {
        thread_data_destroy(data);
        return NULL;
    }
    
    buffer* dados_buff  = buffer_create(1008);
    short data_msg  = 6;
    
    int payload_size = 1000;

    if(sending_window_have_sended(data->window, data->head + data->i)){
        thread_data_destroy(data);
        buffer_destroy(dados_buff);
        return NULL;
    }

    if(data->head + data->i == data->window->frame_count - 1)
        payload_size = data->window->size%FRAME_SIZE;
    
    int sequence = data->head + data->i;

    buffer_set(dados_buff, 0, &data_msg, sizeof(short));
    buffer_set(dados_buff, 2, &sequence, sizeof(int));
    buffer_set(dados_buff, 6, &payload_size, sizeof(short));

    buffer_set(dados_buff, 8, sending_window_get_data_from_head(data->window, data->head + data->i), payload_size);

    udp_client_send(data->udp_client, buffer_get(dados_buff,0), 8+payload_size);  

    buffer_destroy(dados_buff);
    thread_data_destroy(data);

    return NULL;
}