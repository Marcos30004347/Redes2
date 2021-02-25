#include "async.h"

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

typedef void*(*thread_handler)(void*);

struct thread
{
    pthread_t object;
    thread_handler handler;
    void* arg;
};

void thread_create(struct thread** thread, thread_handler handler, void* arg) {
    *thread = (struct thread*)malloc(sizeof(struct thread));
    struct thread* t = *thread;


    t->handler = handler;
    t->arg = arg;

    pthread_create(&t->object, NULL, t->handler, t->arg);
}

void thread_destroy(struct thread* thread)
{
    free(thread);
}

void thread_join(struct thread* thread) {
    pthread_join(thread->object, NULL);
}
