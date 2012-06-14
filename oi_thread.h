// for linux use -pthread

#ifndef OI_THREAD
#define OI_THREAD
#include "oi_os.h"
#include "oi_types.h"


#ifdef OI_WIN
#include<process.h>

typedef struct {
	HANDLE i;
	void (*func)(void*);
	void * data;
} thread_t;

static unsigned int __stdcall _ud_thread_handler(void * args) {
	(*((thread_t*)args)->func)(((thread_t*)args)->data);
	_endthreadex(0);
	return 0;
}

static inline int thread_create(thread_t * t, void (*r)(void*), void * a) {
	t->func = r;
	t->data = a;
	t->i = (HANDLE)_beginthreadex(0,0,&_ud_thread_handler,t,0,0);
	return !t->i;
}

static inline int thread_delete(thread_t * t) {
	return !CloseHandle(t->i);
}

static inline int thread_join(thread_t * t)  {
	return WaitForSingleObject(t->i,INFINITE) != WAIT_OBJECT_0;
}

static inline int thread_yield() {
    Sleep(0);
	return 0;
}

static inline int thread_terminate(thread_t * t) {
	return !TerminateThread(t->i, 0);
}


#else

#include <pthread.h>
#include <stdlib.h>

typedef struct {
	pthread_t i;
	void (*func)(void*);
	void * data;
} thread_t;

void * _ud_thread_handler(void * args) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	(*((thread_t*)args)->func)(((thread_t*)args)->data);
	return 0;
}

static inline int thread_create(thread_t * t, void (*r)(void*), void * a) {
	t->func = f;
	t->data = a;
	return pthread_create(&t->i,0,&_ud_thread_handler,t);
}

static inline int thread_delete() {
	return 0;
}

static inline int thread_join(thread_t * t) {
	return pthread_join(t->i,0);
}

static inline int thread_yield() {
	return sched_yield();
}

static inline int thread_terminate(thread_t * t) {
	return pthread_cancel(t->i);
}


#endif

#endif

