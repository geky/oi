//
// mutex_t
//
// int mutex_create(mutex_t *);
//
// int mutex_destroy(mutex_t);
//
// int mutex_lock(mutex_t);
//
// int mutex_try_lock(mutex_t);
//
// int mutex_unlock(mutex_t);
//

#ifndef UD_MUTEX_H
#define UD_MUTEX_H

#include "ud_os.h"

#ifdef UD_WIN

typedef CRITICAL_SECTION mutex_t;

#define mutex_create(mutex) \
        (InitializeCriticalSection(mutex),0)

#define mutex_destroy(mutex) \
        (DeleteCriticalSection(&(mutex)),0)

#define mutex_lock(mutex) \
        (EnterCriticalSection(&(mutex)),0)

#define mutex_try_lock(mutex) \
        !TryEnterCriticalSection(&(mutex))

#define mutex_unlock(mutex) \
        (LeaveCriticalSection(&(mutex)),0)


#else


typedef pthread_mutex_t mutex_t;

#define mutex_create(mutex) \
        _ud_mutex_init(mutex)

#define mutex_destroy(mutex) \
        pthread_mutex_destroy(&(mutex))

#define mutex_lock(mutex) \
        pthread_mutex_lock(&(mutex))

#define mutex_try_lock(mutex) \
        pthread_mutex_trylock(&(mutex))

#define mutex_unlock(mutex) \
        pthread_mutex_unlock(&(mutex))

int _ud_mutex_init(mutex_t * m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    return pthread_mutex_init(mutex,&attr);
}


#endif

#endif
