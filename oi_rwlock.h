//In current implementation rwlocks are only recursive for read locks
//
#ifndef OI_RWLOCK
#define OI_RWLOCK
#include "oi_os.h"

#ifdef OI_WIN

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
