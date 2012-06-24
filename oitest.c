#include <stdio.h>
#include <string.h>

char memmm[2][40];
volatile char rrr = 0;
#define BEGIN(xx) printf("\n"#xx" :\n")
#define PRINT(tt, test, ss,xx...) {char arrrr[40]; sprintf(arrrr,ss, ##xx);printf("\t%-10s: %-40s %s",#tt,arrrr,test);}
#define TEST(yy) ((yy) ? ": success\n" : (rrr=1, ": FAILURE!\n"))

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
	PRINT(system,TEST(1),"Windows");
#elif defined(OI_LINUX)
	PRINT(system,TEST(1),"Linux");
#elif defined(OI_MAC)
	PRINT(system,TEST(1),"Macintosh");
#elif defined(OI_FreeBSD)
	PRINT(system,TEST(1),"Free BSD");
#elif defined(OI_UNKOWN_OS)
	PRINT(os,TEST(0),"Unknown");
#else
	PRINT(os,TEST(0),"None");
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
	PRINT(int8, TEST(i8 == 0x7f && sizeof(int8) == 1), "size = %-2d max = %d", sizeof(int8), i8);
	PRINT(uint8, TEST(ui8 == 0xff && sizeof(uint8) == 1), "size = %-2d max = %u", sizeof(uint8), ui8);
	PRINT(int16, TEST(i16 == 0x7fff && sizeof(int16) == 2), "size = %-2d max = %d", sizeof(int16), i16);
	PRINT(uint16, TEST(ui16 == 0xffff && sizeof(uint16) == 2), "size = %-2d max = %u", sizeof(uint16), ui16);
	PRINT(int32, TEST(i32 == 0x7fffffff && sizeof(int32) == 4), "size = %-2d max = %d", sizeof(int32), i32);
	PRINT(uint32, TEST(ui32 == 0xffffffff && sizeof(uint32) == 4), "size = %-2d max = %u", sizeof(uint32), ui32);
	PRINT(int64, TEST(i64 == 0x7fffffffffffffffULL && sizeof(int64) == 8), "size = %-2d max = %lld", sizeof(int64), i64);
	PRINT(uint64, TEST(ui64 == 0xffffffffffffffffULL && sizeof(uint64) == 8), "size = %-2d max = %llu", sizeof(uint64), ui64);
        
	PRINT(float32, TEST(sizeof(float32) == 4), "size = %-2d 1/3->%.20f", sizeof(float32), (float32)(1.0/3.0));
	PRINT(float64, TEST(sizeof(float64) == 8), "size = %-2d 1/3->%.20f", sizeof(float64), (float64)(1.0/3.0));
	PRINT(float80, TEST(sizeof(float80) >  8), "size = %-2d 1/3->%.20Lf", sizeof(float80), (float80)(1.0L/3.0L));
	;
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
        PRINT(pack##n, TEST(*(uint8*)data == 0x12), "%s -> %s", MEM(0,&i##n,sizeof i##n), MEM(1,data, sizeof i##n));\
	PRINT(unpack##n, TEST(i##n == o##n && *(uint8*)data == 0x12), "%s <-", MEM(0,&o##n,sizeof i##n));

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
        PRINT(packf##n, TEST((*(uint8*)data & 0xf0) == 0x40), "%g -> %s", (double)if##n, MEM(0,data, sizeof(float##n)));        \
	PRINT(unpackf##n, TEST((int)of##n == (int)if##n), "%g <-", (double)of##n);      \
	ozero##n = unpackf##n(zero);			\
	onan##n = unpackf##n(nan);			\
	oinf##n = unpackf##n(inf##n);			\
	PRINT( ,TEST(ozero##n == 0 && onan##n != onan##n && oinf##n == 1.0/0.0), "0->%g nan->%g inf->%g", (double)ozero##n, (double)onan##n, (double)oinf##n);
       
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
    PRINT(millis, "\n", "before -> %llu", b);
    PRINT(sleep, TEST(!err), "sleep(1500) err %d", err=sleep(1500));
    PRINT( , TEST(!err), "sleep(1000) err %d", err=sleep(1000));
    PRINT( , TEST(!err), "sleep(500)  err %d", err=sleep(500) );
    a = millis();
    PRINT( , TEST((b/100+30) == (a/100)), "after  -> %llu  diff = %llu", a, a-b);
    
}

#include "oi_thread.h"
void testthreadthread(void * a) {
    int err;
    PRINT(, "\n", "I am thread %d", (int)a);
    
    PRINT(yield, "\n", "thread %d yielding", (int)a);
    err = thread_yield();
    PRINT(, TEST(!err), "thread %d yielding err %d", (int)a, err);
    PRINT(sleep, "\n", "thread %d sleep(100)", (int)a);
    err = sleep(100);
    PRINT(, TEST(!err), "thread %d sleeping err %d", (int)a, err);
}


void testthread() {
    BEGIN(oi_thread);
    thread_t t1,t2;
    int err;
    err = thread_create(&t1,&testthreadthread, (void*)1);
    PRINT(create, TEST(!err), "creating thread %d err %d", 1, err);
    err = thread_create(&t2,&testthreadthread, (void*)2);
    PRINT(      , TEST(!err), "creating thread %d err %d", 2, err);
    
    sleep(10);
    
    err = thread_terminate(&t1);
    PRINT(terminate, TEST(!err), "terminating thread %d err %d", 1, err);
    err = thread_join(&t2);
    PRINT(join, TEST(!err), "joining thread %d err %d", 2, err);
    
    
}

int main() {
	printf("\nChecking oi's operability\n");

	testos();
	testtypes();
	testpack();
        testtime();
        testthread();

	if (rrr) printf("\noi has failed on this system.\nchanges must be made for code using oi to work.\n\n");
	else printf("\noi is functional on this system.\n\n");
	return rrr;
}


