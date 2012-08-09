#ifndef OI_PACK
#define OI_PACK 1
#include "oi_os.h"
#include "oi_types.h"

#include <float.h>
#include <string.h>

#ifdef OI_WIN
#include "winsock2.h"
#else
#include "netinet/in.h"
#endif

oi_func void pack(void * b, void * in, size_t inlen) {
    memcpy(b,in,inlen);
}

oi_func void unpack(void * b, void * out, size_t outlen) {
    memcpy(out,b,outlen);
}

oi_func void pack8(void * b, uint8 in) {
    *(uint8*)b = in;
}

oi_func uint8 unpack8(void * b) {
    return *(uint8*)b;
}

oi_func void pack16(void * b, uint16 in) {
    *(uint16*)b = htons(in);
}

oi_func uint16 unpack16(void * b) {
    return ntohs(*(uint16*)b);
}

oi_func void pack32(void * b, uint32 in) {
    *(uint32*)b = htonl(in);
}

oi_func uint32 unpack32(void * b) {
    return ntohl(*(uint32*)b);
}

oi_func void pack64(void * b, uint64 in) {
    pack32(b,(uint32)(in>>32));
    pack32((uint32*)b+1,(uint32)in);
}

oi_func uint64 unpack64(void * b) {
    return ((uint64)unpack32(b)) << 32 | unpack32(((uint32*)b)+1);

}

//if the mantissa sizes matches IEEE specification it is most likely IEEE 754 
// which is what we want

#if defined(__STDC_IEC_559__) || ((__FLT_MANT_DIG__ == 24) && (__DBL_MANT_DIG__ == 53))

//union hack to prevent type punning warning
oi_func void packf32(void * b, float32 in) {
    union {uint32 i; float32 f;} temp;
    temp.f = in;
    pack32(b,temp.i); 
}

oi_func float32 unpackf32(void * b) {
    union {uint32 i; float32 f;} temp;
    temp.i = unpack32(b);
    return temp.f;
}

oi_func void packf64(void * b, float64 in) {
    union {uint64 i; float64 f;} temp;
    temp.f = in;
    pack64(b,temp.i);
}

oi_func float64 unpackf64(void * b) {
    union {uint64 i; float64 f;} temp;
    temp.i = unpack64(b);
    return temp.f;
}

#else

oi_func void packf32(void * b, float32 in) {
    int exp = 0;    
    uint32 temp = 0;

    if (in != in) temp = 0xffffffff;
    else if (in == INFINITY) temp = 0x7f800000;
    else if (in ==-INFINITY) temp = 0xff800000;
    else if (in != 0) {
        if (in < 0.0f) {temp = 0x80000000; in = -in;} 
                
        while (in >= 2.0f) {in /= 2.0f; exp++;}
        while (in <  1.0f) {in *= 2.0f; exp--;}

        temp |= (uint32)((in-1)*(0x00800000+0.5));
        temp |= (exp+127) << 23;
    }

    pack32(b,temp);
}

oi_func float32 unpackf32(void * b) {
    int exp;
    float32 out;
    uint32 temp = unpack32(b);

    if (temp == 0x00000000) return  0.0f;
    if (temp == 0x7f800000) return  (float32)INFINITY;
    if (temp == 0xff800000) return -(float32)INFINITY;
    if ((temp & 0x7f800000)==0x7f800000) return (float32)NAN;
    
    out = ((float32)(temp&0x007fffff))/0x00800000 + 1;
    exp = ((temp>>23) & 0xff) - 127;
    
    while (exp > 0) {out *= 2.0f; exp--;}
    while (exp < 0) {out /= 2.0f; exp++;}

    if (temp & 0x80000000) out = -out;
    return out;
}

oi_func void packf64(void * b, float64 in) {
    int exp = 0;    
    uint64 temp = 0;

    if (in != in) temp = 0xffffffffffffffffULL;
    else if (in == INFINITY) temp = 0x7ff0000000000000ULL;
    else if (in ==-INFINITY) temp = 0xfff0000000000000ULL;
    else if (in != 0.0) {
        if (in < 0) {temp = 0x8000000000000000ULL; in = -in;} 

        while (in >= 2.0) {in /= 2.0; exp++;}
        while (in <  1.0) {in *= 2.0; exp--;}

        temp |= (uint64)((in-1)*(0x0010000000000000ULL+0.5));
        temp |= (uint64)(exp+1023) << 52;
    }

    pack64(b,temp);
}

oi_func float64 unpackf64(void * b) {
    int exp;
    float64 out;
    uint64 temp = unpack64(b);

    if (temp == 0x0000000000000000ULL) return  0.0;
    if (temp == 0x7ff0000000000000ULL) return  INFINITY;
    if (temp == 0xfff0000000000000ULL) return -INFINITY;
    if ((temp & 0x7ff0000000000000ULL)==0x7ff0000000000000ULL) return NAN;
    
    out = ((float64)(temp&0x000fffffffffffffULL))/0x0010000000000000ULL + 1;
    exp = ((temp>>52) & 0x7ff) - 1023;
    
    while (exp > 0) {out *= 2.0; exp--;}
    while (exp < 0) {out /= 2.0; exp++;}

    if (temp & 0x8000000000000000ULL) out = -out;
    return out;
}

#endif

#endif
