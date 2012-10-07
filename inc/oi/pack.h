#ifndef OI_PACK
#define OI_PACK 1

#include "oi/os.h"
#include "oi/types.h"

#include <string.h>

#ifdef OI_CPP
extern "C" {
#endif


// The underlying representation of numerical types can vary widely between 
// different systems. oi_pack provides a solution in the form of 
// pack and unpack functions for converting to and from the network 
// representation of different built in types. 
// 
// ints are converted to Big-Endian and floats are converted to IEEE 
// representations. If the underlying system is already implemented as such, 
// no conversion is performed.

// Maps the given source array to the net array. Simply a wrapper for memcpy.
void pack(void *, void * in, size_t inlen);

// Maps the net array to the given dest array. Simply a wrapper for memcpy.
void unpack(void *, void * out, size_t outlen);

// Maps the given uint8 to the net buffer.
void pack8(void *, uint8 in);

// Maps the net buffer to an uint8.
uint8 unpack8(void *);

// Maps the given uint16 to the net buffer.
void pack16(void *, uint16 in);

// Maps the net buffer to an uint16.
uint16 unpack16(void *);

// Maps the given uint32 to the net buffer.
void pack32(void *, uint32 in);

// Maps the net buffer to an uint32.
uint32 unpack32(void *);

// Maps the given uint64 to the net buffer.
void pack64(void *, uint64 in);

// Maps the net buffer to an uint64.
uint64 unpack64(void *);

// Maps the given float32 to an IEEE repressentation on the net buffer.
void packf32(void *, float32 in);

// Maps the net buffer to a float32.
float32 unpackf32(void *);

// Maps the given float64 to an IEEE repressentation on the net buffer.
void packf64(void *, float64 in);

// Maps the net buffer to a float64.
float64 unpackf64(void *);


#ifdef OI_CPP
}
#endif

#endif
