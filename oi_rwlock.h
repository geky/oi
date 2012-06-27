//In current implementation rwlocks are only recursive for read locks
//
#ifndef OI_RWLOCK
#define OI_RWLOCK
#include "oi_os.h"

#ifdef OI_WIN

//use window's SRW for versions later than windows vista
#if WINVER >= 0x0600

typedef SRWLOCK rwlock_t;

static inline int rwlock_create(rwlock_t * rw) {
	InitializeSRWLock(rw);
	return 0;
}

static inline int rwlock_destroy(rwlock_t * rw) {
	return 0;
}

static inline int rwlock_read_lock(rwlock_t * rw) {
	AcquireSRWLockShared(rw);
	return 0;
}

static inline int rwlock_try_read_lock(rwlock_t * rw) {
	return !TryAcquireSRWLockShared(rw);
}

static inline int rwlock_read_unlock(rwlock_t * rw) {
	ReleaseSRWLockShared(rw);
	return 0;
}

static inline int rwlock_write_lock(rwlock_t * rw) {
	AcquireSRWLockExclusive(rw);
	return 0;
}

static inline int rwlock_try_write_lock(rwlock_t * rw) {
	return !TryAcquireSRWLockExclusive(rw);
}

static inline int rwlock_write_unlock(rwlock_t * rw) {
	ReleaseSRWLockExclusive(rw);
	return 0;
}

#else
//homemade code for when nothing else is available

typedef struct {
	CRITICAL_SECTION count_lock;
	int	count;
	HANDLE count_event;
	CRITICAL_SECTION write_lock;
} rwlock_t;

static inline int rwlock_create(rwlock_t * rw) {
	InitializeCriticalSection(&rw->count_lock);
	InitializeCriticalSection(&rw->write_lock);
	rw->count_event = CreateEvent(0,1,1,0);
	return !rw->count_event;
}

static inline int rwlock_destroy(rwlock_t * rw) {
	DeleteCriticalSection(&rw->count_lock);
	DeleteCriticalSection(&rw->write_lock);
	return !CloseHandle(rw->count_event);
}

static inline int rwlock_read_lock(rwlock_t * rw) {
	EnterCriticalSection(&rw->write_lock);
	EnterCriticalSection(&rw->count_lock);
	if (!rw->count++) ResetEvent(rw->count_event);
	LeaveCriticalSection(&rw->count_lock);
	LeaveCriticalSection(&rw->write_lock);
	return 0;
}

static inline int rwlock_try_read_lock(rwlock_t * rw) {
	if (!TryEnterCriticalSection(&rw->write_lock)) 
		return 1;
	EnterCriticalSection(&rw->count_lock);
	if (!rw->count++) ResetEvent(rw->count_event);
	LeaveCriticalSection(&rw->count_lock);
	LeaveCriticalSection(&rw->write_lock);	
}

static inline int rwlock_read_unlock(rwlock_t * rw) {
	EnterCriticalSection(&rw->count_lock);
	if (!--rw->count) SetEvent(rw->count_event);
	LeaveCriticalSection(&rw->count_lock);
	return 0;
}

static inline int rwlock_write_lock(rwlock_t * rw) {
	EnterCriticalSection(&rw->write_lock);
	if (!rw->count) WaitForSingleObject(rw->count_event, INFINITE);
	return 0;
}

static inline int rwlock_try_write_lock(rwlock_t * rw) {
	if (!TryEnterCriticalSection(&rw->write_lock)) return 1;
	if (!rw->count) WaitForSingleObject(rw->count_event, INFINITE);
	return 0;
}

static inline int rwlock_write_unlock(rwlock_t * rw) {
	LeaveCriticalSection(&rw->write_lock);
	return 0;
}

#endif
#else
#include <pthread.h>

typedef pthread_rwlock_t rwlock_t;

static inline int rwlock_create(rwlock_t * rw) {
	return pthread_rwlock_init(rw,0);
}

static inline int rwlock_destroy(rwlock_t * rw) {
	return pthread_rwlock_destroy(rw);
}

static inline int rwlock_read_lock(rwlock_t * rw) {
	return pthread_rwlock_rdlock(rw);
}

static inline int rwlock_try_read_lock(rwlock_t * rw) {
	return pthread_rwlock_tryrdlock(rw);
}

static inline int rwlock_read_unlock(rwlock_t * rw) {
	return pthread_rwlock_unlock(rw);
}

static inline int rwlock_write_lock(rwlock_t * rw) {
	return pthread_rwlock_wrlock(rw);
}

static inline int rwlock_try_write_lock(rwlock_t * rw) {
	return pthread_rwlock_trywrlock(rw);
}

static inline int rwlock_write_unlock(rwlock_t * rw) {
	return pthread_rwlock_unlock(rw);
}

#endif

#endif
