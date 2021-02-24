
#include "sliding_window.h"

void showbits( unsigned long x )
{
    int i=0;
    for (i = 0; i <= (sizeof(unsigned long) * 8) - 1; i++)
    {
       putchar(x & (1l << i) ? '1' : '0');
    }
    printf("\n");
}

void print_sliding_window(sliding_window* sliding)
{
    printf("%.*sv(%li)\n", (int)sliding->head%64, "                                                                        ", sliding->head);
    showbits(sliding->window);
    printf("\n");
}

void sliding_window_create(sliding_window** sliding, const char* filename, long frame_count)
{
    *sliding = (sliding_window*)malloc(sizeof(sliding_window));
    (*sliding)->buffer = (char**)malloc(sizeof(char*)*64);
    (*sliding)->size = (long*)malloc(sizeof(long)*64);
    (*sliding)->frame_count = frame_count;
    (*sliding)->window = 0;
    (*sliding)->head = 0;

    (*sliding)->file = fopen(filename, "w");

    for(int i=0; i<64; i++)
    {
        (*sliding)->buffer[i] = (char*)malloc(sizeof(char)*1000);
        (*sliding)->size[i] = 0;
    }
}

void sliding_window_destroy(sliding_window* sliding)
{
    fclose(sliding->file);

    for(int i=0; i<sizeof(unsigned long); i++)
        free(sliding->buffer[i]);

    free(sliding);
}

int sliding_window_eof(sliding_window* sliding)
{
    return sliding->head == sliding->frame_count;
}

void sliding_window_ack_frame(sliding_window* sliding, long frame, const char* payload, long len) {
    if(frame < sliding->head) return;

    unsigned long check_mask = 1L << (frame % 64);

    if(check_mask & sliding->window) return;

    sliding->window |= 1L << (frame % 64);
    printf("frame=%li\n", frame);
    memcpy(sliding->buffer[frame % 64], payload, len);
    // printf("2\n");
    sliding->size[frame % 64] = len;
    // printf("3\n");

    // print_sliding_window(sliding);

    unsigned long mask = 1L << (sliding->head % 64);

    while(mask & sliding->window) {
        printf("4\n");
        printf("4 %li\n", sliding->head % 64);
        // printf("4 %s\n", sliding->buffer[sliding->head % 64]);

        fwrite(
            sliding->buffer[sliding->head % 64],
            sizeof(char),
            sliding->size[frame % 64],
            sliding->file
        );
        printf("5\n");

        sliding->window &= ~mask;
        sliding->head = sliding->head + 1;

        mask = 1L << (sliding->head % 64);
    }
}

