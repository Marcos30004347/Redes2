
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>


// char lookup[16][5] = { 
//     "0000", "0001", "0010", "0011", \
//     "0100", "0101", "0110", "0111", \
//     "1000", "1001", "1010", "1011", \
//     "1100", "1101", "1110", "1111" };

// int print(long input) {
//     char output[64];

//     sprintf(output, "%lX", input);
//     int len = strlen(output);
//     printf("%ld = 0x%lx = ", input, input);
//     for(int i=0; i<len; i++) {
//         if (output[i] > '9') output[i] = output[i] - 'A' + 10;
//         else output[i] = output[i] - '0'; // ASCII fuckery
//         printf("%s", lookup[output[i]]);
//     }

//     printf("\n");
//     return 0;
// }

typedef struct {
    // window
    unsigned long frame_count;
    unsigned long window;
    unsigned long head;
} sliding_cache;

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

