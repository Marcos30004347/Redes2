#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer {
    char* buffer;
} buffer;

buffer* buffer_create(unsigned long size);
void buffer_destroy(buffer* buff);
void buffer_set(buffer* buff, unsigned int pos, void* mem, unsigned long len);
void* buffer_get(buffer* buff, unsigned long pos);

#endif