//requires -pthread on posix machines
//Please note rwlocks are not recursive
#ifndef OI_RWLOCK
#define OI_RWLOCK 1
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
//use window's SRW for versions later than windows vista
#if WINVER >= 0x0600

typedef SRWLOCK rwlock_t;

oi_call rwlock_create(rwlock_t * rw) {
    InitializeSRWLock(rw);
    return 0;
}

oi_call rwlock_destroy(rwlock_t * rw) {
    return 0;
}

oi_call rwlock_read_lock(rwlock_t * rw) {
    AcquireSRWLockShared(rw);
    return 0;
}

oi_call rwlock_try_read_lock(rwlock_t * rw) {
    return TryAcquireSRWLockShared(rw) ? 0 : ERROR_BUSY;
}

oi_call rwlock_read_unlock(rwlock_t * rw) {
    ReleaseSRWLockShared(rw);
    return 0;
}

oi_call rwlock_write_lock(rwlock_t * rw) {
    AcquireSRWLockExclusive(rw);
    return 0;
}

oi_call rwlock_try_write_lock(rwlock_t * rw) {
    return TryAcquireSRWLockExclusive(rw) ? 0 : ERROR_BUSY;
}

oi_call rwlock_write_unlock(rwlock_t * rw) {
    ReleaseSRWLockExclusive(rw);
    return 0;
}

#else
//homemade code for when nothing else is available

typedef struct {
    CRITICAL_SECTION count_lock;
    int count;
    HANDLE count_event;
    CRITICAL_SECTION write_lock;
} rwlock_t;

oi_call rwlock_create(rwlock_t * rw) {
    InitializeCriticalSection(&rw->count_lock);
    InitializeCriticalSection(&rw->write_lock);
    rw->count_event = CreateEvent(0,1,1,0);
    return rw->count_event ? 0 : GetLastError();
}

oi_call rwlock_destroy(rwlock_t * rw) {
    DeleteCriticalSection(&rw->count_lock);
    DeleteCriticalSection(&rw->write_lock);
    return CloseHandle(rw->count_event) ? 0 : GetLastError();
}

oi_call rwlock_read_lock(rwlock_t * rw) {
    EnterCriticalSection(&rw->write_lock);
    EnterCriticalSection(&rw->count_lock);
    if (!rw->count++) ResetEvent(rw->count_event);
    LeaveCriticalSection(&rw->count_lock);
    LeaveCriticalSection(&rw->write_lock);
    return 0;
}

oi_call rwlock_try_read_lock(rwlock_t * rw) {
    if (!TryEnterCriticalSection(&rw->write_lock)) 
        return ERROR_BUSY;
    EnterCriticalSection(&rw->count_lock);
    if (!rw->count++) ResetEvent(rw->count_event);
    LeaveCriticalSection(&rw->count_lock);
    LeaveCriticalSection(&rw->write_lock);  
    return 0;
}

oi_call rwlock_read_unlock(rwlock_t * rw) {
    EnterCriticalSection(&rw->count_lock);
    if (!--rw->count) SetEvent(rw->count_event);
    LeaveCriticalSection(&rw->count_lock);
    return 0;
}

oi_call rwlock_write_lock(rwlock_t * rw) {
    EnterCriticalSection(&rw->write_lock);
    EnterCriticalSection(&rw->count_lock);
    if (rw->count) {
        LeaveCriticalSection(&rw->count_lock);
        WaitForSingleObject(rw->count_event, INFINITE);
    } else {
        LeaveCriticalSection(&rw->count_lock);
    }
    return 0;
}

oi_call rwlock_try_write_lock(rwlock_t * rw) {
    if (!TryEnterCriticalSection(&rw->write_lock)) return 1;
        EnterCriticalSection(&rw->count_lock);
    if (rw->count) {
        LeaveCriticalSection(&rw->count_lock);
        LeaveCriticalSection(&rw->write_lock);
        return ERROR_BUSY;
    } else {
        LeaveCriticalSection(&rw->count_lock);
        return 0;
    }
}

oi_call rwlock_write_unlock(rwlock_t * rw) {
    LeaveCriticalSection(&rw->write_lock);
    return 0;
}

#endif
#else
#include <pthread.h>

typedef pthread_rwlock_t rwlock_t;

oi_call rwlock_create(rwlock_t * rw) {
    return pthread_rwlock_init(rw,0);
}

oi_call rwlock_destroy(rwlock_t * rw) {
    return pthread_rwlock_destroy(rw);
}

oi_call rwlock_read_lock(rwlock_t * rw) {
    return pthread_rwlock_rdlock(rw);
}

oi_call rwlock_try_read_lock(rwlock_t * rw) {
    return pthread_rwlock_tryrdlock(rw);
}

oi_call rwlock_read_unlock(rwlock_t * rw) {
    return pthread_rwlock_unlock(rw);
}

oi_call rwlock_write_lock(rwlock_t * rw) {
    return pthread_rwlock_wrlock(rw);
}

oi_call rwlock_try_write_lock(rwlock_t * rw) {
    return pthread_rwlock_trywrlock(rw);
}

oi_call rwlock_write_unlock(rwlock_t * rw) {
    return pthread_rwlock_unlock(rw);
}

#endif

#endif
