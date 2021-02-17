#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "network/tcp_client.h"
#include "network/udp_client.h"
#include "network/signals.h"

#include "signals.h"

int main(int argc, char *argv[]) {
    if(argc < 3) 
    {
        printf("Argumentos insuficientes!\n");
        return -1;
    }

    const char* url = argv[1];
    int port = atoi(argv[2]);
    
    char* arquivo = argv[3];
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
    udp_client_t_create(&udp_client, url, udp_port);

    int packets = size/1000;
    int last = size%1000;

    for(int i=0; i<packets; i++)
    {
        int payload_size = 1000;
        if(i == packets - 1) payload_size = last;
        char payload[payload_size];
        fread(&payload, sizeof(char), payload_size, file);

        char dados[2+4+2+payload_size];
        *((short*)&dados[0]) = 6;
        *((int*)&dados[2]) = i;
        *((short*)&dados[6]) = payload_size;
        memcpy(&dados[8], payload, payload_size);

        char ack[6];
        int received = 0;

        do {
            udp_client_t_send(udp_client, dados, 2+4+2+payload_size);
            received = tcp_client_t_receive(client, ack, 6);
        } while(received == 0 || received == -1);

        if(*((short*)&ack) != 7)
        {
            printf("Invalid response '%i' from server!\n", *((short*)&ack));
            exit(-1);
        }
    }

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