#ifndef OI_TIME
#define OI_TIME
#include "oi_os.h"
#include "oi_types.h"

#ifdef OI_WIN
#include<windows.h>

static inline uint64 millis(void) {
    FILETIME temp;
    GetSystemTimeAsFileTime(&temp);
    uint64 ret = temp.dwHighDateTime;
    ret <<= 32;
    ret |= temp.dwLowDateTime;
    return (ret/10000) - 11644473600000ULL; 
}

static inline int sleep(unsigned int ms) {
	Sleep(ms);
	return 0;	
}

#else

#include <pthread.h>
#include <sys/time.h>

static inline uint64 millis(void) {
    struct timeval temp;
    gettimeofday(&temp,0);
    uint64 ret = temp.tv_sec;
    ret *= 1000;
    ret += (temp.tv_usec/1000);
    return ret;
}

static inline int sleep(unsigned int ms) {
    struct timespec time;
    uint64 timems = millis() + ms;
    time.tv_nsec = (timems%1000) * 1000;
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
}

#endif

#endif

