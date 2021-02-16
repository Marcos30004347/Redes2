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

    struct tcp_client_t* client = NULL;
    tcp_client_t_create(&client, url, port);



    return 0;
}