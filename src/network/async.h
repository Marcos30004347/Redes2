#ifndef NETWORK_ASYNC_H
#define NETWORK_ASYNC_H

struct thread;
struct mutex;

typedef struct thread thread;
typedef struct mutex mutex;

typedef void*(*thread_handler)(void*);

struct thread* thread_create(thread_handler handler, void* arg);
void thread_destroy(struct thread* thread);
void thread_join(struct thread* thread);

struct mutex* mutex_create();
void mutex_lock(mutex* mutex);
void mutex_unlock(mutex* mutex);
void mutex_destroy(mutex* mutex);
#endif