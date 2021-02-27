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

struct mutex
{
    pthread_mutex_t object;
};

struct thread* thread_create(thread_handler handler, void* arg) {
    thread *t = (struct thread*)malloc(sizeof(struct thread));


    t->handler = handler;
    t->arg = arg;

    pthread_create(&t->object, NULL, t->handler, t->arg);
    return t;
}

void thread_destroy(struct thread* thread)
{
    free(thread);
}

void thread_join(struct thread* thread) {
    pthread_join(thread->object, NULL);
}


struct mutex* mutex_create() {
    mutex* m = (mutex*)malloc(sizeof(mutex));
    pthread_mutex_init(&(m->object), NULL);
    return m;
}

void mutex_lock(mutex* mutex) {
    pthread_mutex_lock(&mutex->object);
}

void mutex_unlock(mutex* mutex) {
    pthread_mutex_unlock(&mutex->object);
}

void mutex_destroy(mutex* mutex) {
    free(mutex);
}