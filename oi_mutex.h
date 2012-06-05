#ifndef OI_MUTEX
#define OI_MUTEX
#include "oi_os.h"

#ifdef OI_WIN
#include "windows.h"

typedef CRITICAL_SECTION mutex_t;

static inline int mutex_create(mutex_t * m) {
	InitializeCriticalSection(m);
	return 0;
}

static inline int mutex_destroy(mutex_t * m) {
	DeleteCriticalSection(m);
	return 0;
}

static inline int mutex_lock(mutex_t * m) {
	EnterCriticalSection(m);
	return 0;
}

static inline int mutex_try_lock(mutex_t * m) {
	return !TryEnterCriticalSection(m);
}

static inline int mutex_unlock(mutex_t * m) {
	LeaveCriticalSection(m);
	return 0;
}


#else


typedef pthread_mutex_t mutex_t;

static inline int mutex_create(mutex_t * m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(mutex,&attr);
}

static inline int mutex_destroy(mutex_t * m) {
	return pthread_mutex_destroy(m);
}

static inline int mutex_lock(mutex_t * m) {
	return pthread_mutex_lock(m);
}

static inline int mutex_try_lock(mutex_t * m) {
	return pthread_mutex_trylock(m);
}

static inline int mutex_unlock(mutex_t * m) \
	return pthread_mutex_unlock(m);
}

#endif

#endif
