//requires -pthread in posix
#ifndef OI_COND
#define OI_COND 1

#include "oi/os.h"
#include "oi/mutex.h"

#ifdef OI_CPP
extern "C" {
#endif


// type cond_t

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


oi_call cond_create(cond_t *);

oi_call cond_destroy(cond_t *);

oi_call cond_wait(cond_t *, mutex_t *);

oi_call cond_timed_wait(cond_t *, mutex_t *, unsigned int ms);

oi_call cond_signal_one(cond_t *);

oi_call cond_signal_all(cond_t *);


#ifdef OI_CPP
}
#endif

#endif
