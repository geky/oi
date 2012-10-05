#include "oi/time.h"

#ifdef OI_WIN

uint64 millis(void) {
    FILETIME temp;
    uint64 ret;
    GetSystemTimeAsFileTime(&temp);
    ret = temp.dwHighDateTime;
    ret <<= 32;
    ret |= temp.dwLowDateTime;
    return (ret/10000) - 11644473600000ULL; 
}

#else
#include <sys/time.h>

uint64 millis(void) {
    struct timeval temp;
    uint64 ret;

    gettimeofday(&temp,0);
    ret = temp.tv_sec;
    ret *= 1000;
    ret += (temp.tv_usec/1000);
    return ret;
}

#endif
