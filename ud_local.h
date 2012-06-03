#ifndef UD_LOCAL_H
#define UD_LOCAL_H

#include "ud_os.h"

#ifdef UD_WIN

typedef DWORD local_t;

static inline int local_create(local_t * i) {
	i = TlsAlloc();
	return i == -1;
}

static inline int local_destroy(local_t * i) {
	return !TlsFree(*i);
}

static inline int local_set(local_t * i, void * val) {
	return !TlsSetValue(*i,val);
}

static inline void * local_get(local_t * i) {
	return TlsGetValue(*i);
}

#else


typedef pthread_key_t _t;

static inline int local_create(local_t * i) {
	return pthread_key_create(i,0);
}

static inline int local_destroy(local_t * i) {
	return pthread_delete(*i);
}

static inline int local_set(local_t * i, void * val) {
	return pthread_setspecific(*i,val);
}

static inline void * local_get(local_t * i) {
	return pthread_getspecific(*i);
}


#endif

#endif
