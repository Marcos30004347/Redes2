#ifndef SLIDING_WINDOW_SERVER
#define SLIDING_WINDOW_SERVER

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
    
    // file stuff
    FILE* file;
    char** buffer;
    long* size;
} sliding_window;


void sliding_window_create(sliding_window** sliding, const char* filename, long frame_count);
void sliding_window_destroy(sliding_window* sliding);
int sliding_window_eof(sliding_window* sliding);
void sliding_window_ack_frame(sliding_window* sliding, long frame, const char* payload, long len);
#endif
