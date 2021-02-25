#include "sending_window.h"

void showbits( unsigned long x )
{
    int i=0;
    for (i = 0; i <= (sizeof(unsigned long) * 8) - 1; i++)
    {
       putchar(x & (1l << i) ? '1' : '0');
    }
    printf("\n");
}

void print_sending_window(sending_window* sliding)
{
    printf("%.*sv(%li)\n", (int)sliding->head%(WINDOW_SIZE*2), "                                                                        ", sliding->head);
    showbits(sliding->window);
    printf("\n");
}

void sending_window_create(sending_window** sliding, const char* filename)
{
    *sliding = (sending_window*)malloc(sizeof(sending_window));
    (*sliding)->window = 0;
    (*sliding)->head = 0;
    (*sliding)->file = fopen(filename, "rb");

    fseek((*sliding)->file, 0L, SEEK_END);

    (*sliding)->size = ftell((*sliding)->file);

    fseek((*sliding)->file, 0L, 0);

    (*sliding)->frame_count = (long)ceil((*sliding)->size/(float)FRAME_SIZE);

    (*sliding)->buffer = malloc(sizeof(char*)*WINDOW_SIZE*2);
    for(int i=0; i<WINDOW_SIZE*2; i++)
    {
        (*sliding)->buffer[i] = malloc(sizeof(char)*FRAME_SIZE);
    }

    for(int i=0;i<WINDOW_SIZE; i++)
    {
        int payload_size = FRAME_SIZE;

        if((*sliding)->head + i == (*sliding)->frame_count - 1)
            payload_size = (*sliding)->size%FRAME_SIZE;

        fseek((*sliding)->file, ((*sliding)->head + i)*FRAME_SIZE, SEEK_SET);
        fread((*sliding)->buffer[i], sizeof(char), payload_size, (*sliding)->file);
    }
}

void sending_window_destroy(sending_window* sliding)
{
    fclose(sliding->file);

    for(int i=0; i<WINDOW_SIZE*2; i++)
        free(sliding->buffer[i]);

    free(sliding->buffer);
    free(sliding);
}

int sending_window_eof(sending_window* sliding)
{
    return sliding->head == sliding->frame_count;
}

int sending_window_has_sended(sending_window* sliding, int i)
{
    unsigned long mask = 1L << (sliding->head+i % (WINDOW_SIZE*2));
    return mask & sliding->window;
}

char* sending_window_get_data_from_head(sending_window* sliding, long frame) {
    // printf("%s\n", sliding->buffer[frame % (WINDOW_SIZE*2)]);
    return sliding->buffer[frame%(WINDOW_SIZE*2)];
}

void sending_window_ack_frame(sending_window* sliding, long frame)
{
    if(frame < sliding->head) return;

    unsigned long check_mask = 1L << (frame % (WINDOW_SIZE*2));

    if(check_mask & sliding->window) return;

    sliding->window |= 1L << (frame % (WINDOW_SIZE*2));

    unsigned long mask = 1L << (sliding->head % (WINDOW_SIZE*2));

    while(mask & sliding->window)
    {
        fseek(sliding->file, (sliding->head + WINDOW_SIZE)*FRAME_SIZE, SEEK_SET);

        int payload_size = FRAME_SIZE;
    
        if(sliding->head == sliding->frame_count - 1)
            payload_size = sliding->size%FRAME_SIZE;;
        fread(sliding->buffer[(sliding->head + WINDOW_SIZE) % (WINDOW_SIZE*2)], sizeof(char), payload_size, sliding->file);
        
        sliding->window &= ~mask;
        sliding->head = sliding->head + 1;
       
        mask = 1L << (sliding->head % (WINDOW_SIZE*2));
    }
}

