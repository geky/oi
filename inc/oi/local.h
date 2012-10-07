#ifndef OI_LOCAL
#define OI_LOCAL 1

#include "oi/os.h"

#ifdef OI_CPP
extern "C" {
#endif


// requires "-pthread" on posix machines
//    
// oi_local contains code for storing a pointer in a thread unique manner. 
// Each thread veiws a seperate unique value in the local storage.


// type local_t //
#ifdef OI_WIN
typedef DWORD local_t;
#else
#include <pthread.h>
typedef pthread_key_t local_t;
#endif
//////////////////


// Initializes the local storage. 
// The value will be initialized to a null-pointer for all threads.
oi_call local_create(local_t *);

// Cleans up any resources used by the local storage.
oi_call local_destroy(local_t *);

// Sets the value of the local storage to the value given 
// for only the thread that calls the function.
oi_call local_set(local_t *, void * val);

// Returns the value currently associated with the local storage.
void * local_get(local_t *);


#ifdef OI_CPP
}
#endif

#endif
