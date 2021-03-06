#ifndef OI_TYPES
#define OI_TYPES 1

#include "oi/os.h"

#include <math.h>
#include <float.h>


// oi_types contains the definitions of standard sized numeric types. 
// It is identical to stdint.h but is available 
// outside of C99's standard library.


// types int8   //
//       uint8  //
//       int16  //
//       uint16 //
//       int32  //
//       uint32 //
//       int64  //
//       uint64 //
#if defined(OI_MSVC)
typedef signed char        int8;
typedef unsigned char      uint8;
typedef signed short       int16;
typedef unsigned short     uint16;
typedef signed __int32     int32;
typedef unsigned __int32   uint32;
typedef signed __int64     int64;
typedef unsigned __int64   uint64;
#elif defined(OI_C) && __STDC_HOSTED__
#include <stdint.h>
typedef int8_t             int8;
typedef uint8_t            uint8;
typedef int16_t            int16;
typedef uint16_t           uint16;
typedef int32_t            int32;
typedef uint32_t           uint32;
typedef int64_t            int64;
typedef uint64_t           uint64;
#else
typedef signed char        int8;
typedef unsigned char      uint8;
typedef signed short       int16;
typedef unsigned short     uint16;
typedef signed int         int32;
typedef unsigned int       uint32;
typedef signed long long   int64;
typedef unsigned long long uint64;
#endif
//////////////////


// types float32 //
//       float64 //
typedef float float32;
typedef double float64;
///////////////////


// definitions of INFINITY //
//            and NAN      //
#ifdef OI_MSVC
#pragma warning(disable : 4056)
#pragma warning(disable : 4756)
#define INFINITY (DBL_MAX+DBL_MAX)
#define NAN (INFINITY-INFINITY)
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif
#ifndef NAN
#define NAN (0.0/0.0)
#endif
/////////////////////////////


#endif
