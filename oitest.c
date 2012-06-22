#include <stdio.h>
#include <string.h>

char arrrr[40];
char memmm[2][40];
char rrr = 0;
#define BEGIN(xx) printf("\n"#xx" :\n")
#define PRINT(tt,ss,xx...) {sprintf(arrrr, ss, ##xx); printf("\t%-10s: %-41s", #tt, arrrr);}
#define TEST(yy) {if (yy) printf(": success\n"); else {printf(": FAILURE!\n"); rrr=1;}}

const char * MEM(int b, void * p, int len) {
	int off=0; 
	sprintf(memmm[b],"0x");
	for(; len--; off++) {
		sprintf(memmm[b]+2*off+2,"%02x",*((unsigned char*)p+off));
        }
	return memmm[b];
}

#include "oi_os.h"
void testos() {
	BEGIN(oi_os);
#if defined(OI_WIN)
	PRINT(system,"Windows"); TEST(1);
#elif defined(OI_LINUX)
	PRINT(system,"Linux"); TEST(1);
#elif defined(OI_MAC)
	PRINT(system,"Macintosh"); TEST(1);
#elif defined(OI_FreeBSD)
	PRINT(system,"Free BSD"); TEST(1);
#elif defined(OI_UNKOWN_OS)
	PRINT(os,"Unknown"); TEST(0);
#else
	PRINT(os,"None"); TEST(0);
#endif
}

#include "oi_types.h"
void testtypes(void) {
	uint8  ui8  = (uint8 )-1;
	uint16 ui16	= (uint16)-1;
	uint32 ui32 = (uint32)-1;
	uint64 ui64 = (uint64)-1;
	int8  i8  = (int8 )(ui8 /2);
	int16 i16 = (int16)(ui16/2);
	int32 i32 = (int32)(ui32/2);
	int64 i64 = (int64)(ui64/2);

	BEGIN(oi_types);
	PRINT(int8  ,"size = %-2d max = %d", sizeof(int8), i8);
	TEST(i8 == 0x7f && sizeof(int8) == 1);
	PRINT(uint8 ,"size = %-2d max = %u", sizeof(uint8), ui8);
	TEST(ui8 == 0xff && sizeof(uint8) == 1);
	PRINT(int16 ,"size = %-2d max = %d", sizeof(int16), i16);    
	TEST(i16 == 0x7fff && sizeof(int16) == 2);
	PRINT(uint16,"size = %-2d max = %u", sizeof(uint16), ui16);   
	TEST(ui16 == 0xffff && sizeof(uint16) == 2);
	PRINT(int32 ,"size = %-2d max = %d", sizeof(int32), i32);    
	TEST(i32 == 0x7fffffff && sizeof(int32) == 4);
	PRINT(uint32,"size = %-2d max = %u", sizeof(uint32), ui32);   
	TEST(ui32 == 0xffffffff && sizeof(uint32) == 4);
	PRINT(int64 ,"size = %-2d max = %lld", sizeof(int64), i64);  
	TEST(i64 == 0x7fffffffffffffffULL && sizeof(int64) == 8);
	PRINT(uint64,"size = %-2d max = %llu", sizeof(uint64), ui64); 
	TEST(ui64 == 0xffffffffffffffffULL && sizeof(uint64) == 8);
	PRINT(float32,"size = %-2d 1/3->%.20f", sizeof(float32), (float32)(1.0/3.0));
	TEST(sizeof(float32) == 4);
	PRINT(float64,"size = %-2d 1/3->%.20f", sizeof(float64), (float64)(1.0/3.0));
	TEST(sizeof(float64) == 8);
	PRINT(float80,"size = %-2d 1/3->%.20Lf", sizeof(float80), (float80)(1.0L/3.0L));
	TEST(sizeof(float80) > 8);
}

#include "oi_pack.h"
void testpack() {
#define PACKTEST(n)                             \
	memset(data,0,16);                      \
	pack##n(data,i##n);                     \
	o##n = unpack##n(data);                 \
        PRINT(pack##n, "%s -> %s",              \
		MEM(0,&i##n,sizeof i##n), 	\
		MEM(1,data, sizeof i##n)); 	\
		printf("\n"); 			\
	PRINT(unpack##n, "%s <-",               \
		MEM(0,&o##n,sizeof i##n)); 	\
	TEST(i##n == o##n);

	uint8 data[16];
	uint8 i8 = 0x12, o8 = 0;
	uint16 i16 = 0x1234, o16 = 0;
	uint32 i32 = 0x12345678, o32 = 0;	
	uint64 i64 = 0x123456789abcdef0ULL, o64 = 0;
	float32 if32 = 2.625, of32 = 0;
	float64 if64 = 2.625, of64 = 0;
	float80 if80 = 2.625, of80 = 0;

	BEGIN(oi_pack);
	PACKTEST(8);
	PACKTEST(16);
	PACKTEST(32);
	PACKTEST(64);
	printf("\n");

	memset(data,0,16);
	packf32(data,if32);
	of32 = unpackf32(data);
        PRINT(packf32, "%f -> %s", if32, MEM(0,data, sizeof(float32)));
        printf("\n");
	PRINT(unpackf32, "%f <-", of32);
	TEST(if32 == of32);
        
        memset(data,0,16);
	packf64(data,if64);
	of64 = unpackf64(data);
        PRINT(packf64, "%f -> %s", if64, MEM(0,data, sizeof(float64)));
        printf("\n");
	PRINT(unpackf64, "%f <-", of64);
	TEST(if64 == of64);
        
        memset(data,0,16);
	packf32(data,if80);
	of32 = unpackf80(data);
        PRINT(packf80, "%Lf -> %s", if80, MEM(0,data, sizeof(float80)));
        printf("\n");
	PRINT(unpackf80, "%Lf <-", of80);
	TEST(if80 == of80);
        

#undef PACKTEST
}



int main() {
	printf("\nChecking oi's operability\n");

	testos();
	testtypes();
	testpack();	

	if (rrr) printf("\noi has failed on this system.\nchanges must be made for code realiant on oi to work.\n\n");
	else printf("\noi is functional on this system.\n\n");
	return 0;
}


