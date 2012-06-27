//requires -pthread on posix machines
#ifndef OI_MUTEX
#define OI_MUTEX 1
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN

typedef CRITICAL_SECTION mutex_t;

oi_call mutex_create(mutex_t * m) {
    InitializeCriticalSection(m);
    return 0;
}

oi_call mutex_destroy(mutex_t * m) {
    DeleteCriticalSection(m);
    return 0;
}

oi_call mutex_lock(mutex_t * m) {
    EnterCriticalSection(m);
    return 0;
}

oi_call mutex_try_lock(mutex_t * m) {
    return !TryEnterCriticalSection(m);
}

oi_call mutex_unlock(mutex_t * m) {
    LeaveCriticalSection(m);
    return 0;
}


#else
#include <pthread.h>

typedef pthread_mutex_t mutex_t;

oi_call mutex_create(mutex_t * m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(m,&attr);
}

oi_call mutex_destroy(mutex_t * m) {
    return pthread_mutex_destroy(m);
}

oi_call mutex_lock(mutex_t * m) {
    return pthread_mutex_lock(m);
}

oi_call mutex_try_lock(mutex_t * m) {
    return pthread_mutex_trylock(m);
}

oi_call mutex_unlock(mutex_t * m) {
    return pthread_mutex_unlock(m);
}

#endif

#endif
