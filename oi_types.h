#ifndef OI_TYPES
#define OI_TYPES

#ifndef _MSC_VER
#include "stdint.h"
typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef int64_t  int64;
typedef uint64_t uint64;
#else
typedef signed char      int8;
typedef unsigned char    uint8;
typedef signed short     int16;
typedef unsigned short   uint16;
typedef signed int       int32;
typedef unsigned int     uint32;
typedef signed __int64   int64;
typedef unsigned __int64 uint64;
#endif

typedef float float32;
typedef double float64;

//please note float80 support on most systems tends to be subpar.
//the biggest instance is windows in which float80 is as precise as float32
typedef long double float80;

#endif
