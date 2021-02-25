
#include "receiving_window.h"

void showbits( unsigned long x )
{
    int i=0;
    for (i = 0; i <= (sizeof(unsigned long) * 8) - 1; i++)
    {
       putchar(x & (1l << i) ? '1' : '0');
    }
    printf("\n");
}

void print_receiving_window(receiving_window* sliding)
{
    printf("%.*sv(%li)\n", (int)sliding->head%64, "                                                                        ", sliding->head);
    showbits(sliding->window);
    printf("\n");
}

void receiving_window_create(receiving_window** sliding, const char* filename, long frame_count)
{
    printf("asdasda\n");
    *sliding = (receiving_window*)malloc(sizeof(receiving_window));
    (*sliding)->buffer = (char**)malloc(sizeof(char*)*64);
    (*sliding)->size = (long*)malloc(sizeof(long)*64);
    (*sliding)->frame_count = frame_count;
    (*sliding)->window = 0;
    (*sliding)->head = 0;

    (*sliding)->file = fopen(filename, "w");

    printf("asdasda\n");
    for(int i=0; i<WINDOW_SIZE * 2; i++)
    {
        (*sliding)->buffer[i] = (char*)malloc(sizeof(char)*FRAME_SIZE);
        (*sliding)->size[i] = 0;
    }
    printf("asdasda\n");
}

void receiving_window_destroy(receiving_window* sliding)
{
    fclose(sliding->file);

    for(int i=0; i<sizeof(unsigned long); i++)
        free(sliding->buffer[i]);

    free(sliding);
}

int receiving_window_eof(receiving_window* sliding) {
    return sliding->head == sliding->frame_count;
}

void receiving_window_ack_frame(receiving_window* sliding, long frame, const char* payload, long len) {
    if(frame < sliding->head) return;

    unsigned long check_mask = 1L << (frame % (WINDOW_SIZE * 2));

    if(check_mask & sliding->window) return;

    sliding->window |= 1L << (frame % (WINDOW_SIZE * 2));
    // printf("frame=%li\n", frame);
    memcpy(sliding->buffer[frame % (WINDOW_SIZE * 2)], payload, len);
    // // printf("2\n");
    sliding->size[frame % (WINDOW_SIZE * 2)] = len;
    // // printf("3\n");

    // print_receiving_window(sliding);

    unsigned long mask = 1L << (sliding->head % (WINDOW_SIZE * 2));

    while(mask & sliding->window) {
        // printf("4\n");
        // printf("4 %li\n", sliding->head % (WINDOW_SIZE * 2));
        // // printf("4 %s\n", sliding->buffer[sliding->head % (WINDOW_SIZE * 2)]);

        fwrite(
            sliding->buffer[sliding->head % (WINDOW_SIZE * 2)],
            sizeof(char),
            sliding->size[frame % (WINDOW_SIZE * 2)],
            sliding->file
        );
        // printf("5\n");

        sliding->window &= ~mask;
        sliding->head = sliding->head + 1;

        mask = 1L << (sliding->head % (WINDOW_SIZE * 2));
    }
}

