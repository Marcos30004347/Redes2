all: server client


server:
	gcc \
	-fno-stack-protector \
	src/server.c \
	src/network/tcp_server.c \
	src/network/udp_server.c \
	src/network/async.c \
	src/sliding-window/sliding_window.c \
	src/sliding-window/buffer.c \
	-o servidor -lpthread -lm

client:
	gcc \
	-fno-stack-protector \
	src/client.c \
	src/network/tcp_client.c \
	src/network/udp_client.c \
	src/network/async.c \
	src/network/utils.c \
	src/sliding-window/sliding_cache.c \
	-o cliente -lpthread -lm

