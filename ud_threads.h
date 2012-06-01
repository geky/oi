#include "ud_os.h"

//
// thread_t
//
// bool thread_create(thread_t * thread, void (* routine)(void *), void * args)
//
// bool thread_join(thread_t thread)
//
// bool thread_terminate(thread_t thread);
//
// bool thread_delete(thread_t thread);
//

#ifdef WINDOWS_UD

typedef HANDLE thread_t;

#define thread_create(thread, routine, args) \
	!(*(thread) = _beginthreadex(0,0,&_ud_thread_handler,_ud_thread_args(routine,args),0,0))		
#define thread_join(thread) \
	WaitForSingleObject(thread, INFINITE)
#define thread_terminate(thread) \
	!TerminateThread(thread, 0)
#define thread_delete(thread) \
	!CloseHandle(thread);

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
	pthread_create(thread,0,&_ud_thread_handler,_ud_thread_args(routine,args))
#define thread_join(thread) \
	pthread_join(thread,0)
#define thread_terminate(thread) \
	pthread_cancel(thread)
#define thread_delete 0

void * _ud_thread_handler(void ** args) {
	if (!args) return 0;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	((void(*)(void*))(args[0]))(args[1]);
	free(args);
	return 0;
}

#endif

void * _ud_thread_args(void (*routine)(void*), void * args) {
	void ** mem = malloc(sizeof(routine)+sizeof(args));
	mem[0] = routine;
	mem[1] = args;
	return mem;
}
