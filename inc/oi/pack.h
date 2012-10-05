#ifndef OI_PACK
#define OI_PACK 1

#include "oi/os.h"
#include "oi/types.h"

#ifdef OI_CPP
extern "C" {
#endif


void pack(void *, void * in, size_t inlen);

void unpack(void *, void * out, size_t outlen);

void pack8(void *, uint8 in);

uint8 unpack8(void *);

void pack16(void *, uint16 in);

uint16 unpack16(void *);

void pack32(void *, uint32 in);

uint32 unpack32(void *);

void pack64(void *, uint64 in);

uint64 unpack64(void *);

void packf32(void *, float32 in);

float32 unpackf32(void *);

void packf64(void *, float64 in);

float64 unpackf64(void *);


#ifdef OI_CPP
}
#endif

#endif
