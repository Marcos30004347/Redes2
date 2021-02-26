all: server client

setup-trafic:
	sudo tc qdisc add dev lo root netem rate 1mbit limit 20 delay 10ms loss 10%

cleanup-trafic:
	sudo tc qdisc del dev lo root

server:
	gcc \
	src/sliding-window/buffer.c \
	src/server.c \
	src/network/tcp_server.c \
	src/network/udp_server.c \
	src/network/async.c \
	src/sliding-window/receiving_window.c \
	-o servidor -lpthread -lm

client:
	gcc \
	src/client.c \
	src/network/tcp_client.c \
	src/sliding-window/buffer.c \
	src/network/udp_client.c \
	src/network/async.c \
	src/network/utils.c \
	src/sliding-window/sending_window.c \
	-o cliente -lpthread -lm

