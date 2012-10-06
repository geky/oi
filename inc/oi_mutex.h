// requires -pthread on posix machines
#ifndef OI_MUTEX
#define OI_MUTEX 1

#include "oi_os.h"

#ifdef OI_CPP
extern "C" {
#endif


// type mutex_t;

#ifdef OI_WIN
typedef CRITICAL_SECTION mutex_t;
#else
#include <pthread.h>
typedef pthread_mutex_t mutex_t;
#endif


oi_call mutex_create(mutex_t*);

oi_call mutex_destroy(mutex_t*);

oi_call mutex_lock(mutex_t*);

// returns ERR_IN_USE on failure
oi_call mutex_try_lock(mutex_t*);

oi_call mutex_unlock(mutex_t*);


#ifdef OI_CPP
}
#endif

#endif
