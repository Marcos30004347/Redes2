#ifndef RECEIVING_WINDOW_SERVER
#define RECEIVING_WINDOW_SERVER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>

#include "constants.h"

typedef struct {
    unsigned long frame_count;
    unsigned long window;
    unsigned long head;
    
    FILE* file;
    char** buffer;
    long* size;
} receiving_window;


void receiving_window_create(receiving_window** sliding, const char* filename, long frame_count);
void receiving_window_destroy(receiving_window* sliding);
int receiving_window_eof(receiving_window* sliding);
void receiving_window_ack_frame(receiving_window* sliding, long frame, const char* payload, long len);
#endif
