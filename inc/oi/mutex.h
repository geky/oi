#ifndef OI_MUTEX
#define OI_MUTEX 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// requires "-pthread" on posix machines
//
// oi_mutex contains the code for cross-platform, recursive 
// mutual exclusion locks. useful for creating data barriers around 
// data shared between threads.

    
// type mutex_t //
#ifdef OI_WIN
typedef CRITICAL_SECTION mutex_t;
#else
#include <pthread.h>
typedef pthread_mutex_t mutex_t;
#endif
//////////////////


// Initializes a recursive mutex.
oi_call mutex_create(mutex_t*);

// Cleans up resources used by the mutex.
oi_call mutex_destroy(mutex_t*);

// Locks the given mutex, or waits until it is unlocked by another thread.
oi_call mutex_lock(mutex_t*);

// Locks the given mutex, or returns ERR_IN_USE if locked in another thread.
oi_call mutex_try_lock(mutex_t*);

// Unlocks the given mutex, allowing other threads to lock it.
oi_call mutex_unlock(mutex_t*);


#ifdef OI_CPP
}
#endif

#endif
