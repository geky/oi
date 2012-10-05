// requires -pthread on posix machines
#ifndef OI_LOCAL
#define OI_LOCAL 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// type local_t

#ifdef OI_WIN
typedef DWORD local_t;
#else
#include <pthread.h>
typedef pthread_key_t local_t;
#endif


oi_call local_create(local_t *);

oi_call local_destroy(local_t *);

oi_call local_set(local_t *, void * val);

void * local_get(local_t *);


#ifdef OI_CPP
}
#endif

#endif
