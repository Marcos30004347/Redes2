#ifndef SLIDING_WINDOW_CLIENT
#define SLIDING_WINDOW_CLIENT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>

typedef struct {
    // window
    unsigned long frame_count;
    unsigned long window;
    unsigned long head;
} sliding_cache;


void sliding_cache_create(sliding_cache** sliding, long frame_count);
void sliding_cache_destroy(sliding_cache* sliding);
int sliding_cache_eof(sliding_cache* sliding);
int sliding_cache_has_sended(sliding_cache* sliding, int i);
void sliding_cache_ack_frame(sliding_cache* sliding, long frame);

#endif
