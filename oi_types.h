//
// int8
// uint8
// int16
// uint16
// int32
// uint32
// int64
// uint64
//
// float32
// float64
//

#ifndef OI_TYPES
#define OI_TYPES

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;

#ifdef _MSC_VER
typedef signed __int64 int64;
typedef unsigned __int64 uint64;
#else
typedef signed long long int64;
typedef unsigned long long uint64;
#endif

typedef float float32;
typedef double float64;

#endif
