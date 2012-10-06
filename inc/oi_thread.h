//requires -pthread on posix machines
#ifndef OI_THREAD
#define OI_THREAD 1

#include "oi_os.h"

#ifdef OI_CPP
extern "C" {
#endif


// type thread_t
#ifdef OI_WIN
typedef struct {
    HANDLE i;
    void (*func)(void*);
    void * data;
} thread_t;
#else
#include <pthread.h>
typedef struct {
    pthread_t i;
    void (*func)(void*);
    void * data;
} thread_t;
#endif


oi_call thread_create(thread_t *, void (*routine)(void*), void * args);

oi_call thread_sleep(unsigned int ms);

oi_call thread_yield(void);

oi_call thread_join(thread_t *);

oi_call thread_terminate(thread_t *);


#ifdef OI_CPP
}
#endif

#endif

