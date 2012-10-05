#include "oi/rwlock.h"

#ifdef OI_WIN
#if WINVER >= 0x0600

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
    if (!TryEnterCriticalSection(&rw->write_lock)) 
        return ERROR_BUSY;
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
