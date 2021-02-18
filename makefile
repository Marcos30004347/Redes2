all: server client


server:
	gcc \
	server.c \
	network/tcp_server.c \
	network/udp_server.c \
	network/async.c \
	-o servidor -lpthread -lm

client:
	gcc \
	client.c \
	network/tcp_client.c \
	network/udp_client.c \
	network/async.c \
	-o cliente -lpthread -lm

window-simulator:
	gcc \
	simulate_window.c \
	-o window