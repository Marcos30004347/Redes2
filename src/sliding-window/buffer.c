#include "buffer.h"
#include <stdlib.h>
#include <string.h>

buffer* buffer_create(unsigned int size) {
    buffer* buff = (buffer*)malloc(sizeof(buffer*));
    buff->buffer = (char*)malloc(sizeof(char)*size);
    return buff;
}

void buffer_destroy(buffer* buff) {
    free(buff->buffer);
    free(buff);
}

void buffer_set(buffer* buff, unsigned int pos, void* mem, unsigned int len) {
    memcpy(buff->buffer + pos, mem, len);
}

void* buffer_get(buffer* buff, unsigned int pos) {
    return (void*)&buff->buffer[pos];
}
