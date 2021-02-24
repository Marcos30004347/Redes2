#include "sliding_cache.h"

void showbits( unsigned long x )
{
    int i=0;
    for (i = 0; i <= (sizeof(unsigned long) * 8) - 1; i++)
    {
       putchar(x & (1l << i) ? '1' : '0');
    }
    printf("\n");
}

void print_sliding_cache(sliding_cache* sliding)
{
    printf("%.*sv(%li)\n", (int)sliding->head%64, "                                                                        ", sliding->head);
    showbits(sliding->window);
    printf("\n");
}

void sliding_cache_create(sliding_cache** sliding, long frame_count)
{
    *sliding = (sliding_cache*)malloc(sizeof(sliding_cache));
    (*sliding)->frame_count = frame_count;
    (*sliding)->window = 0;
    (*sliding)->head = 0;

}

void sliding_cache_destroy(sliding_cache* sliding)
{
    free(sliding);
}

int sliding_cache_eof(sliding_cache* sliding)
{
    return sliding->head == sliding->frame_count;
}

int sliding_cache_has_sended(sliding_cache* sliding, int i)
{
    unsigned long mask = 1L << (sliding->head+i % 64);
    return mask & sliding->window;
}

void sliding_cache_ack_frame(sliding_cache* sliding, long frame)
{
    if(frame < sliding->head) return;

    unsigned long check_mask = 1L << (frame % 64);

    if(check_mask & sliding->window) return;

    sliding->window |= 1L << (frame % 64);

    // print_sliding_cache(sliding);

    unsigned long mask = 1L << (sliding->head % 64);

    while(mask & sliding->window)
    {
        sliding->window &= ~mask;
        sliding->head = sliding->head + 1;
        mask = 1L << (sliding->head % 64);
    }
}

