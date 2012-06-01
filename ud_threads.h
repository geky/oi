#include "ud_os.h"

//
// thread_t
//
// int thread_create(thread_t * thread, void (* routine)(void *), void * args)
//
// int thread_join(thread_t thread)
//
// int thread_terminate(thread_t thread);
//
// int thread_delete(thread_t thread);
//

#ifdef WINDOWS_UD

typedef HANDLE thread_t;

#define thread_create(thread, routine, args) \
	_ud_thread_init(thread,routine,args)
#define thread_join(thread) \
	WaitForSingleObject(thread, INFINITE)
#define thread_terminate(thread) \
	!TerminateThread(thread, 0)
#define thread_delete(thread) \
	!CloseHandle(thread);

int _ud_thread_init(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	mem[0] = r;  mem[1] = a;
	*t = _beginthreadex(0,0,&_ud_thread_handler,mem,0,0);
	return !*t;
}

unsigned int __stdcall _ud_thread_handler(void ** args) {
	if (!args) _endthreadex(0);
	((void(*)(void*))(args[0]))(args[1]);
	free(args);
	_endthreadex(0);
	return 0;
}

#else

typedef pthread_t thread_t;


#define thread_create(thread, routine, args) \
	_ud_thread_init(thread,routine,args)
#define thread_join(thread) \
	pthread_join(thread,0)
#define thread_terminate(thread) \
	pthread_cancel(thread)
#define thread_delete 0

int _ud_thread_init(thread_t * t, void (*r)(void*), void * a) {
	void ** mem = malloc(sizeof r + sizeof a);
	mem[0] = r;  mem[1] = a;
	return pthread_create(thread,0,&_ud_thread_handler,mem);
}

void * _ud_thread_handler(void ** args) {
	if (!args) return 0;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	((void(*)(void*))(args[0]))(args[1]);
	free(args);
	return 0;
}

#endif

