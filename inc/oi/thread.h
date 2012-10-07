#ifndef OI_THREAD
#define OI_THREAD 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// requires "-pthread" on posix machines
//
// oi_thread contains code for creating and handling threads
// independantly of the underlying OS.
//
// Threads are complex entities not fully described here. If you are new 
// to threads you should research other implementations to understand the 
// bugs that multithreaded code can induce.


// type thread_t //
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
///////////////////


// Creates a new thread, and runs the routine with the argument that is given.
oi_call thread_create(thread_t *, void (*routine)(void*), void * args);

// Makes the current thread sleep the amount given in milliseconds.
oi_call thread_sleep(unsigned int ms);

// Makes the current thread yield to allow other threads a chance to run.
oi_call thread_yield(void);

// Waits for the given thread to complete and 
// then cleans up the thread's resources.
oi_call thread_join(thread_t *);

// Forcefully destroys the given thread. This can cause a leak in memory 
// if the thread doesn't get a chance to clean up its resources, and 
// it can happen anywhere and lead to surprising bugs. So it should only 
// be used as a last resort or where heap issues won't matter, such as 
// closing an application suddenly.
oi_call thread_terminate(thread_t *);


#ifdef OI_CPP
}
#endif

#endif

