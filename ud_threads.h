//
// thread_t
//
// int thread_create(thread_t * thread, void (* routine)(void *), void * args)
//
// int thread_delete(thread_t thread)
//
// int thread_join(thread_t thread)
//
// int thread_sleep(uint32 millis)
//
// int thread_yeild()
//
// int thread_terminate(thread_t thread)
//

#ifndef UD_THREAD_H
#define UD_THREAD_H

#include "ud_os.h"
#include "ud_types.h"


#ifdef UD_WIN

#include<windows.h>
#include<process.h>

typedef HANDLE thread_t;

#define thread_create(thread, routine, args) \
        _ud_thread_init(thread,routine,args)

#define thread_delete(thread) \
	!CloseHandle(thread)

#define thread_join(thread) \

#define thread_yeild() \
        (Sleep(0),0)

#define thread_terminate(thread) \
	!TerminateThread(thread, 0)



static unsigned int __stdcall _ud_thread_handler(void * args) {
	if (!args) _endthreadex(0);
        ((void(**)(void*))args)[0](((void**)args)[1]);
	free(args);
	_endthreadex(0);
	return 0;
}

int _ud_thread_init(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	mem[0] = r;  mem[1] = a;
	*t = (thread_t)_beginthreadex(0,0,&_ud_thread_handler,mem,0,0);
	return !*t;
}



#else


typedef pthread_t thread_t;


#define thread_create(thread, routine, args) \
	_ud_thread_init(thread,routine,args)

#define thread_delete() 0

#define thread_join(thread) \
	pthread_join(thread,0)

#define thread_sleep(millis) \
        _ud_thread_sleep(millis)

#define thread_yeild() \
        pthread_yeild()

#define thread_terminate(thread) \
	pthread_cancel(thread)

void * _ud_thread_handler(void * args) {
	if (!args) return 0;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	((void(**)(void*))args)[0](((void**)args)[1]);
	free(args);
	return 0;
}

int _ud_thread_init(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	mem[0] = r;  mem[1] = a;
	return pthread_create(thread,0,&_ud_thread_handler,mem);
}


#endif

#endif

