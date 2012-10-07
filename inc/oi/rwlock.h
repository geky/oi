#ifndef OI_RWLOCK
#define OI_RWLOCK 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// requires "-pthread" on posix machines
//
// oi_rwlock contains code for cross-platform read-write locks that
// allow mutiple readers to access data if there are no writers currently 
// using the lock, and only allows one writer to get a lock.
//
// Please note that these locks are not recursive, and the prioritization 
// scheme is dependent on the underlying system.


// type rwlock_t //
#ifdef OI_WIN
#if WINVER >= 0x0600
typedef SRWLOCK rwlock_t;
#else
typedef struct {
    CRITICAL_SECTION count_lock;
    int count;
    HANDLE count_event;
    CRITICAL_SECTION write_lock;
} rwlock_t;
#endif
#else
#include <pthread.h>
typedef pthread_rwlock_t rwlock_t;
#endif
///////////////////


// Initialized a non-recursive read-write lock.
oi_call rwlock_create(rwlock_t*);

// Cleans up resources used by the rwlock.
oi_call rwlock_destroy(rwlock_t*);

// Adds a reading lock on the given rwlock, or 
// waits until a writer is unlocked in another threads.
oi_call rwlock_read_lock(rwlock_t*);

// Adds a reading lock on the given rwlock, or 
// returns ERR_IN_USE if it is locked by a writer in another thread.
oi_call rwlock_try_read_lock(rwlock_t*);

// Unlocks a read lock, allowing a writer to lock
// if there are no other readers.
oi_call rwlock_read_unlock(rwlock_t*);

// Makes a write lock on the given rwlock, or 
// waits until all readers are unlocked in other threads.
oi_call rwlock_write_lock(rwlock_t*);

// Makes a write lock on the given rwlock, or 
// returns ERR_IN_USE if it is locked by any readers in other threads.
oi_call rwlock_try_write_lock(rwlock_t*);

// Releases the write lock on rw, allowing readers to lock, or another 
// writer to lock depending on the prioritization scheme of the system.
oi_call rwlock_write_unlock(rwlock_t*);


#ifdef OI_CPP
}
#endif

#endif
