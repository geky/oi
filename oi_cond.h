//requires -pthread in posix
#ifndef OI_COND
#define OI_COND 1
#include "oi_os.h"
#include "oi_mutex.h"

#ifdef OI_WIN
#if WINVER >= 0x0600

typedef CONDITION_VARIABLE cond_t;

oi_call cond_create(cond_t * c) {
    InitializeConditionVariable(c);
    return 0;
}

oi_call cond_destroy(cond_t * c) {
    return 0;
}

oi_call cond_wait(cond_t * c, mutex_t * m) {
    return SleepConditionVariableCS(c,m,INFINITE) ? GetLastError() : 0;
}

// returns ERR_TIMEOUT on timeout
oi_call cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    return SleepConditionVariableCS(c,m,ms) ? GetLastError() : 0;
}

oi_call cond_signal_one(cond_t * c) {
    WakeConditionVariable(c);
    return 0;
}

oi_call cond_signal_all(cond_t * c) {
    WakeAllConditionVariable(c);
    return 0;
}

#else 
//homemade solution for windows before vista

typedef struct {
    HANDLE event;
    CRITICAL_SECTION lock;
    int unlocking;
    int count;
} cond_t;

oi_call cond_create(cond_t * c) {
    c->count = 0;
    c->unlocking = 0;
    c->event = CreateEvent(0,1,1,0);
    InitializeCriticalSection(&c->lock);
    return c->event ? 0 : GetLastError();
}

oi_call cond_destroy(cond_t * c) {
    DeleteCriticalSection(&c->lock);
    return CloseHandle(c->event) ? 0 : GetLastError();
}

oi_call cond_wait(cond_t * c, mutex_t * m) {
    EnterCriticalSection(&c->lock);
    c->count++;
    LeaveCriticalSection(&c->lock);
    LeaveCriticalSection(m);

    while(1) {
        Sleep(0); //give other threads a chance to consume signal
        WaitForSingleObject(c->event,INFINITE);

        EnterCriticalSection(&c->lock);
        if (c->unlocking) {
            if(!--c->unlocking)
                ResetEvent(c->event);
            c->count--;
            LeaveCriticalSection(&c->lock);
            EnterCriticalSection(m);
            return 0;
        }
        LeaveCriticalSection(&c->lock);
    }
}

// returns ERR_TIMEOUT on timeout
oi_call cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    EnterCriticalSection(&c->lock);
    c->count++;
    LeaveCriticalSection(&c->lock);
    LeaveCriticalSection(m);

    while (1) {
        Sleep(0); //give other threads a chance to consume signal
        if (WaitForSingleObject(c->event,ms) == WAIT_TIMEOUT) {
            EnterCriticalSection(&c->lock);
            c->count--;
            LeaveCriticalSection(&c->lock);
            EnterCriticalSection(m);
            return ERROR_TIMEOUT;
        }

    
        EnterCriticalSection(&c->lock);
        if (c->unlocking) {
            if (!--c->unlocking)
                ResetEvent(c->event);
            c->count--;
            LeaveCriticalSection(&c->lock);
            EnterCriticalSection(m);
            return 0;
        }
        LeaveCriticalSection(&c->lock);
    }
}

oi_call cond_signal_one(cond_t * c) {
    EnterCriticalSection(&c->lock);
    c->unlocking = 1;
    SetEvent(c->event);
    LeaveCriticalSection(&c->lock);
    return 0;
}

oi_call cond_signal_all(cond_t * c) {
    EnterCriticalSection(&c->lock);
    c->unlocking = c->count;
    SetEvent(c->event);
    LeaveCriticalSection(&c->lock);
    return 0;
}

#endif

#else

#include <sys/time.h>

typedef pthread_cond_t cond_t;

oi_call cond_create(cond_t * c) {
    return pthread_cond_init(c,0);
}

oi_call cond_destroy(cond_t * c) {
    return pthread_cond_destroy(c);
}

oi_call cond_wait(cond_t * c, mutex_t * m) {
    return pthread_cond_wait(c,m);
}

// returns ERR_TIMEOUT on timeout
oi_call cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    struct timespec time;
    struct timeval temp;
    
    gettimeofday(&temp,0);
    ms += (temp.tv_usec/1000);
    time.tv_nsec = (ms%1000) * 1000000;
    time.tv_sec = (ms/1000) + temp.tv_sec;
       
    return pthread_cond_timedwait(c,m,&time);
}

oi_call cond_signal_one(cond_t * c) {
    return pthread_cond_signal(c);
}

oi_call cond_signal_all(cond_t * c) {
    return pthread_cond_broadcast(c);
}

#endif

#endif
