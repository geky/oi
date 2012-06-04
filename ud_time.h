//
// uint64 millis()
//
// int sleep(int millis)
//

#ifndef UD_TIME_H
#define UD_TIME_H
#include "ud_os.h"
#include "ud_types.h"

#ifdef UD_WIN

#include<windows.h>

#define millis() \
      	_ud_millis()

#define sleep(millis) \
        (Sleep(millis),0)

uint64 _ud_millis() {
    FILETIME temp;
    GetSystemTimeAsFileTime(&temp);
    uint64 ret = temp.dwHighDateTime;
    ret <<= 32;
    ret |= temp.dwLowDateTime;
    return (ret/10000) - 11644473600000ULL; 
}

#else

#include <pthread.h>
#include <sys/time.h>

#define millis() \
      	_ud_millis()

#define sleep(millis) \
        _ud_sleep(millis)

uint64 _ud_millis() {
    struct timeval temp;
    gettimeofday(&temp,0);
    uint64 ret = temp.tv_sec;
    ret *= 1000;
    ret += (temp.tv_usec/1000);
    return ret;
}

int _ud_sleep(unsigned int millis) {
    struct timespec time;
    uint64 timems = millis();
    timems += millis;
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

