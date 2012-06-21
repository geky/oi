#ifndef OI_PACK
#define OI_PACK

//#include "oi_os.h"
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
	return ntohs(*(uint16*)b);
}

static inline void pack32(void * b, uint32 in) {
	*(uint32*)b = htonl(in);
}

static inline uint32 unpack32(void * b) {
	return ntohl(*(uint32*)b);
}

static inline void pack64(void * b, uint64 in) {
	pack32(b,(uint32)(in>>32));
	pack32(b,(uint32)in);
}

static inline uint64 unpack64(void * b) {
	return ((uint64)unpack32(b)) << 32 | unpack32(((uint32*)b)+1);

}

#include <float.h>
//if the mantissa sizes matches IEEE specification it is most likely IEEE 754 
// which is what we want

#if defined(__STDC_IEC_559__) || (__FLT_MANT_DIG__ == 23)

static inline void packf32(void * b, float32 in) {
	pack32(b,in); 
}

static inline float32 unpackf32(void * b) {
	return unpack32(b);
}

#else

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

		temp |= (uint32)((in-1)*(0x00800000+0.5f));
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


#endif

#if defined(__STDC_IEC_559__) || (__DBL_MANT_DIG__ == 53)

static inline void packf64(void * b, float64 in) {
	pack64(b,in);
}

static inline float64 unpackf64(void * b) {
	return unpack64(b);
}

#else

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

//just manually putting into IEEE float form due to overabundance of issues with padding, endianess, and floating point implementations.... 
//only on a 16 bit machine would you be able to directly pack the raw value. 
//I would optimize this later but if you're using extended precision floats you probably aren't focused on speed.
static inline void packf80(void * b, float80 in) {
	int shift = 0;	
        uint16 exp = 0;
	uint64 mant = 0;

	if (in != in) {exp = 0xffff; mant = 0xffffffffffffffffULL;}
	else if (in == ( 1.0L/0.0L)) {exp = 0x7fff; mant = 0x8000000000000000ULL;}
	else if (in == (-1.0L/0.0L)) {exp = 0xffff; mant = 0x8000000000000000ULL;}
	else if (in != 0) {
		if (in < 0) {exp = 0x8000; in = -in;} 

		while (in >= 2.0L) {in /= 2.0L; shift++;}
		while (in <  1.0L) {in *= 2.0L; shift--;}

		mant = in*(0x8000000000000000ULL+0.5L);
		exp |= shift + 16383;
	}

	pack16(b,exp);
	pack64(b,mant);
}

static inline float80 unpackf80(void * b) {
        int shift = 0;
	uint16 exp = unpack16(b);
	uint64 mant = unpack64(b);
	float80 out;

	if (!exp && !mant) return 0.0L;
	else if(((exp&0x7fff)==0x7fff) && mant == 0xffffffffffffffffULL) return 0.0L/0.0L;
	else if(exp == 0x7fff && mant == 0x8000000000000000ULL) return  1.0L/0.0L;
	else if(exp == 0xffff && mant == 0x8000000000000000ULL) return -1.0L/0.0L;

	out = mant / 0x8000000000000000ULL;
	if (exp & 0x8000) out = -out;
	shift = exp & 0x7fff - 16383;
	
	while (shift > 0) {out *= 2.0L; shift--;}
	while (shift < 0) {out /= 2.0L; shift++;}

	return out;
}

#endif

