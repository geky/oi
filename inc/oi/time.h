#ifndef OI_TIME
#define OI_TIME 1

#include "oi/os.h"
#include "oi/types.h"

#ifdef OI_CPP
extern "C" {
#endif


// oi_time provides a time related functions useful for time dependant code.


// Returns the current time in milliseconds.
uint64 millis(void);


#ifdef OI_CPP
}
#endif

#endif
