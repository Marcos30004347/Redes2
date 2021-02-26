#ifndef SENDING_WINDOW_CLIENT
#define SENDING_WINDOW_CLIENT

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "constants.h"

typedef struct {
    // window
    unsigned long frame_count;
    unsigned long window;
    unsigned long head;

    FILE* file;
    char** buffer;
    long size;
    long framecount;
} sending_window;

sending_window* sending_window_create(const char* filename);
void sending_window_destroy(sending_window* sliding);
int sending_window_eof(sending_window* sliding);
int sending_window_has_sended(sending_window* sliding, int i);
void sending_window_ack_frame(sending_window* sliding, long frame);
char* sending_window_get_data_from_head(sending_window* sliding, long frame);
#endif
