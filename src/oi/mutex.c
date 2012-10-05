#include "oi/mutex.h"

#ifdef OI_WIN

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
    return TryEnterCriticalSection(m) ? 0 : ERROR_BUSY;
}

oi_call mutex_unlock(mutex_t * m) {
    LeaveCriticalSection(m);
    return 0;
}

#else

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
