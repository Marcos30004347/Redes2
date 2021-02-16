all: server client


server:
	gcc \
	server.c \
	network/tcp_server.c \
	network/udp_server.c \
	network/async.c \
	-o servidor -lpthread

client:
	gcc \
	client.c \
	network/tcp_client.c \
	network/udp_client.c \
	network/async.c \
	-o cliente -lpthread