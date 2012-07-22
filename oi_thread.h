//requires -pthread on posix machines
#ifndef OI_THREAD
#define OI_THREAD 1
#include "oi_os.h"
#include "oi_types.h"


#ifdef OI_WIN
#include <process.h>

typedef struct {
    HANDLE i;
    void (*func)(void*);
    void * data;
} thread_t;

static unsigned int __stdcall _oi_thread_handler(void * args) {
    (*((thread_t*)args)->func)(((thread_t*)args)->data);
    _endthreadex(0);
    return 0;
}

oi_call thread_create(thread_t * t, void (*r)(void*), void * a) {
    t->func = r;
    t->data = a;
    t->i = (HANDLE)_beginthreadex(0,0,&_oi_thread_handler,t,0,0);
    return !t->i;
}

oi_call thread_destroy(thread_t * t) {
    return !CloseHandle(t->i);
}

oi_call thread_join(thread_t * t)  {
    return WaitForSingleObject(t->i,INFINITE) != WAIT_OBJECT_0;
}

oi_call thread_sleep(unsigned int ms) {
    Sleep(ms);
    return 0;   
}

oi_call thread_yield(void) {
    Sleep(0);
    return 0;
}

oi_call thread_terminate(thread_t * t) {
    return !TerminateThread(t->i, 0);
}

#else

#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct {
    pthread_t i;
    void (*func)(void*);
    void * data;
} thread_t;

void * _oi_thread_handler(void * args) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    (*((thread_t*)args)->func)(((thread_t*)args)->data);
    return 0;
}

oi_call thread_create(thread_t * t, void (*r)(void*), void * a) {
    t->func = r;
    t->data = a;
    return pthread_create(&t->i,0,&_oi_thread_handler,t);
}

oi_call thread_destroy(thread_t * t) {
    return 0;
}

oi_call thread_join(thread_t * t) {
    return pthread_join(t->i,0);
}

oi_call thread_sleep(unsigned int ms) {
    struct timespec time;
    struct timeval temp;
    uint64 timems;

    gettimeofday(&temp,0);
    timems = temp.tv_sec;
    timems *= 1000;
    timems += (temp.tv_usec/1000);

    timems += ms;
    time.tv_nsec = (timems%1000) * 1000000;
    time.tv_sec = timems / 1000;
    
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_init(&mutex,0);
    pthread_cond_init(&cond,0);
    
    pthread_mutex_lock(&mutex);
    pthread_cond_timedwait(&cond,&mutex,&time);
    pthread_mutex_unlock(&mutex);
    
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
	return 0;
}

oi_call thread_yield(void) {
    return sched_yield();
}

oi_call thread_terminate(thread_t * t) {
    return pthread_cancel(t->i);
}

#endif

#endif

