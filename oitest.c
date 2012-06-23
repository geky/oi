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
	uint8 data[16];
	uint8 i8 = 0x12, o8 = 0;
	uint16 i16 = 0x1234, o16 = 0;
	uint32 i32 = 0x12345678, o32 = 0;	
	uint64 i64 = 0x123456789abcdef0ULL, o64 = 0;
	float32 if32 = 12.625, of32 = 0;
	float32 onan32 = 0, ozero32 = 0, oinf32 = 0;
	float64 if64 = 12.625, of64 = 0;	
	float32 onan64 = 0, ozero64 = 0, oinf64 = 0;
	float80 if80 = 12.625, of80 = 0;	
	float32 onan80 = 0, ozero80 = 0, oinf80 = 0;
	uint8  nan[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	uint8 zero[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8 inf32[4] = {0x7f,0x80,0x00,0x00};
	uint8 inf64[8] = {0x7f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8 inf80[10]= {0x7f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#define PACKTEST(n)                             \
	memset(data,0,16);                      \
	pack##n(data,i##n);                     \
	o##n = unpack##n(data);                 \
        PRINT(pack##n, "%s -> %s",              \
		MEM(0,&i##n,sizeof i##n), 	\
		MEM(1,data, sizeof i##n)); 	\
	TEST(*(uint8*)data == 0x12);            \
	PRINT(unpack##n, "%s <-",               \
		MEM(0,&o##n,sizeof i##n)); 	\
	TEST(i##n == o##n && *(uint8*)data == 0x12);

	BEGIN(oi_pack);
	PACKTEST(8);
	PACKTEST(16);
	PACKTEST(32);
	PACKTEST(64);
	printf("\n");

#undef PACKTEST
#define PACKTEST(n)                                     \
	memset(data,0,16);                              \
	packf##n(data,if##n);                           \
	of##n = unpackf##n(data);                       \
        PRINT(packf##n, "%g -> %s", (double)if##n,      \
		MEM(0,data, sizeof(float##n)));		\
        TEST((*(uint8*)data & 0xf0) == 0x40);		\
	PRINT(unpackf##n, "%g <-", (double)of##n);	\
	TEST((int)of##n == (int)if##n);			\
	ozero##n = unpackf##n(zero);			\
	onan##n = unpackf##n(nan);			\
	oinf##n = unpackf##n(inf##n);			\
	PRINT( , "0->%g nan->%g inf->%g",               \
		(double)ozero##n, (double)onan##n, (double)oinf##n);	\
	TEST(ozero##n == 0 && onan##n != onan##n && oinf##n == 1.0/0.0);		
       
        PACKTEST(32);
	PACKTEST(64);    
	PACKTEST(80);

#undef PACKTEST
}

#include "oi_time.h"
void testtime() {
    int err;
    uint64 b,a;
    
    BEGIN(oi_time);
    b=millis();
    PRINT(millis, "before -> %llu", b); printf("\n");
    PRINT(sleep, "sleep(1500) err -> %d", err=sleep(1500)); TEST(!err);
    PRINT(     , "sleep(1000) err -> %d", err=sleep(1000)); TEST(!err);
    PRINT(     , "sleep(500)  err -> %d", err=sleep(500) ); TEST(!err);
    a = millis();
    PRINT(      , "after  -> %llu  diff -> %llu", a, a-b);
    TEST((b/100+30) == (a/100));
    
}



int main() {
	printf("\nChecking oi's operability\n");

	testos();
	testtypes();
	testpack();
        testtime();

	if (rrr) printf("\noi has failed on this system.\nchanges must be made for code using oi to work.\n\n");
	else printf("\noi is functional on this system.\n\n");
	return rrr;
}


