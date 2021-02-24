#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
    char* buffer;
} buffer;

buffer* buffer_create(unsigned int size);
void buffer_destroy(buffer* buff);
void buffer_set(buffer* buff, unsigned int pos, void* mem, unsigned int len);
void* buffer_get(buffer* buff, unsigned int pos);

#endif