#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

buffer* buffer_create(unsigned long size) {
    buffer* buff = (buffer*)malloc(sizeof(buffer*));
    buff->buffer = (char*)malloc(sizeof(char) * size);

    // printf("size %li\n", size);
    // printf("created %p\n", buff);
    // printf("buff mem %p\n", buff->buffer);

    return buff;
}

void buffer_destroy(buffer* buff) {
    free(buff->buffer);
    free(buff);
}

void buffer_set(buffer* buff, unsigned int pos, void* mem, unsigned long len) {
    memcpy((void*)((unsigned long)buff->buffer + pos), mem, len);
}

void* buffer_get(buffer* buff, unsigned long pos) {
    // printf("BUFFER %p\n", buff);
    // printf("GET %p\n", buff->buffer);
    // printf("GET %p\n", (void*)((unsigned long)buff->buffer + pos));

    return (void*)((unsigned long)buff->buffer + pos);
}
