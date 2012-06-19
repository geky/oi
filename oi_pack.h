#ifndef OI_PACK
#define OI_PACK

#include "oi_types.h"

static inline void pack8(void * b, uint8 in) {
	*(uint8*)b = in;
}

static inline uint8 unpack8(void * b) {
	return *(uint8*)b;
}

static inline void pack16(void * b, uint16 in) {
	*(uint16*)b = htons(in);
}

static inline uint16 unpack16(void * b) {
	return ntohs(*(uint16*)in);
}

static inline void pack32(void * b, uint32 in) {
	*(uint32*)b = htonl(in);
}

static inline uint32 unpack32(void * b) {
	return ntohl(*(uint32*)in);
}

static inline void pack64(void * b, uint64 in) {
	pack32(b,(uint32)(in>>32));
	pack32(b,(uint32)in);
}

static inline uint64 unpack64(void * b) {
	return unpack32(b) << 32 |
		   unpack32(((uint32*)b)+1);

}

#ifdef __STDC_IEC_559__

static inline void packf32(void * b, float32 in) {
	pack32(b,in); 
}

static inline float32 unpackf32(void * b) {
	return unpack32(b);
}

static inline void pack64(void * b, float64 in) {
	pack64(b,in);
}

static inline float64 unpack64(void * b) {
	return unpack64(b);
}

#else
#warning does not support IEEE floats, using oi fallback code that is untested

static inline void packf32(void * b, float32 in) {
	int exp = 0;	
	uint32 temp = 0;

	if (in != in) temp = 0xffffffff;
	else if (in == ( 1.0f/0.0f)) temp = 0x7f800000;
	else if (in == (-1.0f/0.0f)) temp = 0xff800000;
	else if (in != 0) {
		if (in < 0) {temp = 0x80000000; in = -in;} 

		while (in >= 2.0f) {in /= 2.0f; exp++;}
		while (in <  1.0f) {in *= 2.0f; exp--;}

		temp |= (in-1)*(0x00800000+0.5f);
		temp |= (exp+127) << 23;
	}

	pack32(b,temp);
}

static inline float32 unpackf32(void * b) {
	int exp;
	float32 out;
	uint32 temp = unpack32(b);

	switch (temp) {
		case 0x00000000: return  0.0f;
		case 0xffffffff: 
		case 0x7fffffff: return  0.0f/0.0f;
		case 0x7f800000: return  1.0f/0.0f;
		case 0xff800000: return -1.0f/0.0f;
	}
	
	out = (temp&0x007fffff)/0x00800000 + 1;
	exp = (temp>>23) & 0xff - 127;
	
	while (exp > 0) {out *= 2.0f; exp--;}
	while (exp < 0) {out /= 2.0f; exp++;}

	if (temp & 0x80000000) out = -out;
	return out;
}

static inline void packf64(void * b, float64 in) {
	int exp = 0;	
	uint64 temp = 0;

	if (in != in) temp = 0xffffffffffffffffULL;
	else if (in == ( 1.0/0.0)) temp = 0x7ff0000000000000ULL;
	else if (in == (-1.0/0.0)) temp = 0xfff0000000000000ULL;
	else if (in != 0) {
		if (in < 0) {temp = 0x8000000000000000ULL; in = -in;} 

		while (in >= 2.0) {in /= 2.0; exp++;}
		while (in <  1.0) {in *= 2.0; exp--;}

		temp |= (in-1)*(0x0010000000000000ULL+0.5);
		temp |= (exp+1023) << 52;
	}

	pack64(b,temp);
}

static inline float64 unpackf64(void * b) {
	int exp;
	float64 out;
	uint64 temp = unpack64(b);

	switch (temp) {
		case 0x0000000000000000ULL: return  0.0;
		case 0xffffffffffffffffULL: 
		case 0x7fffffffffffffffULL: return  0.0/0.0;
		case 0x7ff0000000000000ULL: return  1.0/0.0;
		case 0xfff0000000000000ULL: return -1.0/0.0;
	}
	
	out = (temp&0x000fffffffffffffULL)/0x0010000000000000ULL + 1;
	exp = (temp>>52) & 0x7ff - 1023;
	
	while (exp > 0) {out *= 2.0; exp--;}
	while (exp < 0) {out /= 2.0; exp++;}

	if (temp & 0x8000000000000000ULL) out = -out;
	return out;
}

#endif

#endif

