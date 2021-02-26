#ifndef NETWORK_ASYNC_H
#define NETWORK_ASYNC_H

struct thread;

typedef struct thread thread;

typedef void*(*thread_handler)(void*);

void thread_create(struct thread** thread, thread_handler handler, void* arg);
void thread_destroy(struct thread* thread);
void thread_join(struct thread* thread);

#endif