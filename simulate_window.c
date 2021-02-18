#include <stdio.h>
#include "sliding_window.h"

#define MULTIPLIER 10
#define WINDOW 10
#define FRAMES WINDOW*MULTIPLIER

int main(int argc, char *argv[]) {
    sliding_window* window;
    sliding_window_create(&window, "windowtesting.txt", FRAMES);

    for(int i=0; i<FRAMES/WINDOW; i++)
    {
        unsigned long mask = 0;
        unsigned long max = 0;

        for(int j=0; j<WINDOW; j++)
        {
            max |= 1L << j;
        }

        while(mask != max)
        {
            int j = rand() % WINDOW;
            mask |= 1L << (j);
            sliding_window_ack_frame(window, j + (i*WINDOW), "teste\n", 6);
        }
    }

    printf("%i\n", sliding_window_eof(window));
    printf("%i\n", (int)ceil(10/3.f));

    return 0;
}