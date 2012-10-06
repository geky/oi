// requires -pthread on posix machines
#ifndef OI_RWLOCK
#define OI_RWLOCK 1

#include "oi_os.h"

#ifdef OI_CPP
extern "C" {
#endif


// type rwlock_t

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


oi_call rwlock_create(rwlock_t*);

oi_call rwlock_destroy(rwlock_t*);

oi_call rwlock_read_lock(rwlock_t*);

oi_call rwlock_try_read_lock(rwlock_t*);

oi_call rwlock_read_unlock(rwlock_t*);

oi_call rwlock_write_lock(rwlock_t*);

// returns ERR_IN_USE on failure
oi_call rwlock_try_write_lock(rwlock_t*);

oi_call rwlock_write_unlock(rwlock_t*);


#ifdef OI_CPP
}
#endif

#endif
