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

static inline void packf32(void * b, float32 in) {
	int shift = 0;	
	uint32 temp = 0;

	switch(fpclassify(in)) {
		case FP_NAN: temp = 0x7fffffff; break;
		case FP_INFINITE: temp = 0x7f800000 | ((in<0)<<31); break;
		case FP_NORMAL: case FP_SUBNORMAL:

			if (in < 0) {temp = 0x80000000; in = -in;} 

			while(in >= 2.0) {in /= 2.0; shift++;}
			while(in < 1.0) {in *= 2.0; shift--;}

			temp |= (--in)*(0x00800000+0.5f);
			temp |= (shift+0x7F) << 29;
			break;
		}
	}

	pack32(b,temp);
}

static inline float32 unpackf32(void * b) {
	uint32 temp = unpack32(b);
	
		
}

static inline void pack64(void * b, float64 f) {

}

static inline float64 unpack64(void * b) {

}

#endif

#endif

