// requires -pthread on posix machines
#ifndef OI_LOCAL
#define OI_LOCAL 1
#include "oi_os.h"

#ifdef OI_WIN

typedef DWORD local_t;

oi_call local_create(local_t * i) {
    *i = TlsAlloc();
    return *i==TLS_OUT_OF_INDEXES ? GetLastError() : 0;
}

oi_call local_destroy(local_t * i) {
    return TlsFree(*i) ? 0 : GetLastError();
}

oi_call local_set(local_t * i, void * val) {
    return TlsSetValue(*i,val) ? 0 : GetLastError();
}

oi_func void * local_get(local_t * i) {
    return TlsGetValue(*i);
}

#else
#include <pthread.h>

typedef pthread_key_t local_t;

oi_call local_create(local_t * i) {
    return pthread_key_create(i,0);
}

oi_call local_destroy(local_t * i) {
    return pthread_key_delete(*i);
}

oi_call local_set(local_t * i, void * val) {
    return pthread_setspecific(*i,val);
}

oi_func void * local_get(local_t * i) {
    return pthread_getspecific(*i);
}

#endif

#endif
