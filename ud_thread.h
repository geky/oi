#ifndef UD_THREAD_H
#define UD_THREAD_H

#include "ud_os.h"
#include "ud_types.h"


#ifdef UD_WIN

#include<windows.h>
#include<process.h>

typedef HANDLE thread_t;

static unsigned int __stdcall _ud_thread_handler(void * args) {
	((void(**)(void*))args)[0](((void**)args)[1]);
	free(args);
	_endthreadex(0);
	return 0;
}

static inline int thread_create(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	if (!mem) return 1;
	mem[0] = r;  mem[1] = a;
	*t = (thread_t)_beginthreadex(0,0,&_ud_thread_handler,mem,0,0);
	return !*t;
}


static inline int thread_delete(thread_t * t) {
	return !CloseHandle(&t);
}

static inline int thread_join(thread_t * t)  {
	return WaitForSingleObject(&t,INFINITE) != WAIT_OBJECT_0;
}

static inline int thread_yield() {
    Sleep(0);
	return 0;
}

static inline int thread_terminate(thread_t * t) {
	return !TerminateThread(&t, 0);
}


#else

#include <pthread.h>
#include <stdlib.h>

typedef pthread_t thread_t;

void * _ud_thread_handler(void * args) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	((void(**)(void*))args)[0](((void**)args)[1]);
	free(args);
	return 0;
}

static inline int thread_create(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	if (!mem) return 1;
	mem[0] = r;  mem[1] = a;
	return pthread_create(t,0,&_ud_thread_handler,mem);
}

static inline int thread_delete() {
	return 0;
}

static inline int thread_join(thread) {
	return pthread_join(thread,0);
}

static inline int thread_yield() {
	return sched_yield();
}

static inline int thread_terminate(thread) {
	return pthread_cancel(thread);
}


#endif

#endif

