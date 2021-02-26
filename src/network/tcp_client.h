#ifndef NETWORKCP_CLIENT_H
#define NETWORKCP_CLIENT_H


struct tcp_client;

typedef struct tcp_client tcp_client;

typedef void(*client_handler)(tcp_client*, char*);


void tcp_client_send(tcp_client* client, void* message, int len);

tcp_client* tcp_client_create(const char* url, int port);

void tcp_client_destroy(tcp_client* client);

void tcp_client_disconnect(tcp_client* client);

int tcp_client_receive(tcp_client* client, void* message, int length);

void tcp_client_set_timeout(tcp_client* client, unsigned long ms);

void tcp_client_remove_timeout(tcp_client* client);


#endif