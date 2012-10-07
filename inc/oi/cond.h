#ifndef OI_COND
#define OI_COND 1

#include "oi/os.h"
#include "oi/mutex.h"

#ifdef OI_CPP
extern "C" {
#endif


//requires -pthread in posix
//
// oi_cond contains code for cross-platform condition variables that 
// allow threads to sleep in a state that can be awoken by other threads 
// with an atomically locked mutex.
//
// Please note that all implementations of cond can be susceptible to spurious
// wakeups when no thread has signalled the cond.


// type cond_t //
#ifdef OI_WIN
#if WINVER >= 0x0600
typedef CONDITION_VARIABLE cond_t;
#else 
typedef struct {
    HANDLE event;
    CRITICAL_SECTION lock;
    int unlocking;
    int count;
} cond_t;
#endif
#else
#include <pthread.h>
typedef pthread_cond_t cond_t;
#endif
/////////////////


// Initializes a condition variable in c.
oi_call cond_create(cond_t *);

// Cleans up resources used by the condition variable.
oi_call cond_destroy(cond_t *);

// Makes the currently running thread sleep until the given cond is
// signalled by another thread. The mutex is unlocked by cond_wait and 
// then atomically locked before waking up to prevent a signal from being 
// lost while the cond is locked.
oi_call cond_wait(cond_t *, mutex_t *);

// Makes the currently running thread sleep for the given time in milliseconds, 
// or the given cond is signalled by another thread. If the cond times out, 
// an error of ERR_TIMEOUT is returned. The mutex is unlocked by cond_wait and
// then atomically locked before waking up to prevent a signal from being 
// lost while the cond is locked.
oi_call cond_timed_wait(cond_t *, mutex_t *, unsigned int ms);

// Wakes up a single thread currently waiting on the condition variable.
oi_call cond_signal_one(cond_t *);

// Wakes up all threads currently waiting on the condition variable.
oi_call cond_signal_all(cond_t *);


#ifdef OI_CPP
}
#endif

#endif
