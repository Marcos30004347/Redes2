#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "network/tcp_client.h"
#include "network/udp_client.h"

#include "signals.h"
#include "sliding-window/sliding_cache.h"



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

    FILE* file = fopen(arquivo,"rb");
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, 0);

    struct tcp_client_t* client = NULL;
    tcp_client_t_create(&client, url, port);

    short hello = 1;
    tcp_client_t_send(client, &hello, sizeof(short));

    char connection[6];

    while(tcp_client_t_receive(client, connection, 6) == 0) {}

    if(*((short*)&connection) != 2)
    {
        printf("Invalid response '%i' from server!\n", *((short*)&connection));
        exit(-1);
    }

    int udp_port = *((int*)&connection[2]);
    
    char info_file[25];
    *((short*)&info_file[0]) = MESSAGE_INFO_FILE;
    printf("%li\n", size);
    printf("%li\n", strlen(arquivo));
    printf("%s\n", (arquivo));

    memcpy(&info_file[2], arquivo, strlen(arquivo) + 1);
    *((long*)&info_file[17]) = size;
    
    tcp_client_t_send(client, &info_file, 25);

    short ok;

    while(tcp_client_t_receive(client, &ok, 2) == 0) {}

    if(ok != 4)
    {
        printf("Invalid response '%i' from server!\n", ok);
        exit(-1);
    }

    // Send data
    printf("Envinando dados...\n");


    struct udp_client_t* udp_client = NULL;
    printf("port %i\n", udp_port);
    printf("arquivo %s\n", arquivo);
    udp_client_t_create(&udp_client, url, udp_port);

    long frame_size = 1000;
    long frame_count = (long)ceil(size/(float)frame_size);
    int last = size%frame_size;

    sliding_cache* window;
    sliding_cache_create(&window, frame_count);


    while(!sliding_cache_eof(window))
    {
        for(int i=0; i < 32 && window->head + i < frame_count; i++)
        {
            int payload_size = 1000;
            // printf("SENDING %i...\n", window->head + i);
    
            if(sliding_cache_has_sended(window, window->head + i)) continue;
            
            if(window->head + i == frame_count - 1) payload_size = last;

            
            char dados[8+payload_size];
            *((short*)&dados[0]) = 6;
            *((int*)&dados[2]) = window->head + i;
            *((short*)&dados[6]) = payload_size;
    
            fseek(file, (window->head + i)*1000, SEEK_SET);
            fread(&dados[8], sizeof(char), payload_size, file);
            // printf("enviando\n");
            udp_client_t_send(udp_client, dados, 8+payload_size);
            // printf("enviei!\n");
        }

        char ack[6];
        int k = 0;
        while(!sliding_cache_eof(window) && tcp_client_t_receive(client, ack, 6) && k < 31)
        {
            k++;
            if(*((short*)&ack) != 7)
            {
                printf("Invalid response '%i' from server!\n", *((short*)&ack));
                exit(-1);
            }

    
            unsigned sequence =*(unsigned*)&ack[2];
            sliding_cache_ack_frame(window, sequence);
        }
    }

    sliding_cache_destroy(window);

    fclose(file);
    short fim;

    while(tcp_client_t_receive(client, &fim, 2) == 0) {}

    printf("Fim!\n");

    if(fim != 5)
    {
        printf("Invalid response '%i' from server!\n", fim);
        exit(-1);
    }

    return 0;
}