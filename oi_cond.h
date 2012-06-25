#ifndef OI_COND
#define OI_COND
#include "oi_os.h"
#include "oi_types.h"
#include "oi_mutex.h"

#ifdef OI_WIN
#if WINVER >= 0x0600

typedef CONDITION_VARIABLE cond_t;

static inline int cond_create(cond_t * c) {
	InitializeConditionVariable(c);
	return 0;
}

static inline int cond_destroy(cond_t * c) {
	return 0;
}

static inline int cond_wait(cond_t * c, mutex_t * m) {
	return !SleepConditionVariableCS(c,m,INFINITE);
}

static inline int cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    return !SleepconditionVariableCS(c,m,ms);
}

static inline int cond_signal_one(cond_t * c) {
	WakeConditionVariable(c);
	return 0;
}

static inline int cond_signal_all(cond_t * c) {
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

static inline int cond_create(cond_t * c) {
	c->cound = 0;
	c->unlocking = 0;
	c->event = CreateEvent(0,1,1,0);
	InitializeCriticalSection(&c->lock);
	return 0;
}

static inline int cond_destroy(cond_t * c) {
	DeleteCriticalSection(&c->lock);
	return CloseHandle(c->event);
}

static inline int cond_wait(cond_t * c, mutex_t * m) {
	EnterCriticalSection(&c->lock);
	c->count++;
	LeaveCriticalSection(&c->lock);
	LeaveCriticalSection(m);

	while(1) {
		WaitForSingleObject(c->event,INFINITE);

		EnterCriticalSection(m);
		EnterCriticalSection(&c->lock);
		if (c->unlocking) {
			if(!--c->unlocking)
				ResetEvent(c->event);
			c->count--;
			LeaveCriticalSection(&c->lock);
			return 0;
		}
		LeaveCriticalSection(&c->lock);
		LeaveCriticalSection(m);
	}
}

static inline int cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    EnterCriticalSection(&c->lock);
	c->count++;
	LeaveCriticalSection(&c->lock);
	LeaveCriticalSection(m);

	while(1) {
		if (WaitForSingleObject(c->event,ms) == WAIT_TIMEOUT) {
			EnterCriticalSection(m);
			EnterCriticalSection(&c->lock);
			c->count--;
			LeaveCriticalSection(&c->lock);
			return 1;
		}

		EnterCriticalSection(m);
		EnterCriticalSection(&c->lock);
		if (c->unlocking) {
			if (!--c->unlocking)
				ResetEvent(c->event);
			c->count--;
			LeaveCriticalSection(&c->lock);
			return 0;
		}
		LeaveCriticalSection(&c->lock);
		LeaveCriticalSection(m);
	}
}

static inline int cond_signal_one(cond_t * c) {
	EnterCriticalSection(&c->lock);
	SetEvent(c->event);
	c->unlocking = 1;
	LeaveCriticalSection(&c->lock);
	return 0;
}

static inline int cond_signal_all(cond_t * c) {
	EnterCriticalSection(&c->lock);
	SetEvent(c->event);
	c->unlocking = c->count;
	LeaveCriticalSection(&c->lock);
	return 0;
}


#endif

#else

typedef pthread_cond_t cond_t;

static inline int cond_create(cond_t * c) {
	return pthread_cond_init(c,0);
}

static inline int cond_destroy(cond_t * c) {
	return pthread_cond_destroy(c);
}

static inline int cond_wait(cond_t * c, mutex_t * m) {
	return pthread_cond_wait(c,m);
}

static inline int cond_timed_wait(cond_t * c, mutex_t * m, unsigned int ms) {
    struct timespec time;
    uint64 timems = millis() + ms;
    time.tv_nsec = (timems%1000) * 1000000;
    time.tv_sec = timems / 1000;
	   
    return pthread_cond_timedwait(c,m,&time);
}

static inline int cond_signal_one(cond_t * c) {
	return pthread_cond_signal(c);
}

static inline int cond_signal_all(cond_t * c) {
	return pthread_cond_broadcast(c);
}

#endif

#endif

