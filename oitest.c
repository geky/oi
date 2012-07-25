#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>

char memmm[2][40];
volatile char rrr = 0;
#define TEST(yy) ((yy) ? ": success\n" : (rrr=1, ": FAILURE!\n"))

void PRINT(const char * tt, const char * test, const char * ss, ...) {
    va_list args;
    char arrrr[80]; 
    va_start(args,ss);
    vsprintf(arrrr,ss,args); 
    va_end(args);
    printf("\t%-10s: %-41s%s",tt,arrrr,test);
}

const char * MEM(int b, void * p, int len) {
    char * str = memmm[b];
    unsigned char * dat = (unsigned char*)p;
    sprintf(str,"0x");
    for(str+=2; len--; str+=2, dat++) {
        sprintf(str,"%02x",*dat);
    }
    return memmm[b];
}

#include "oi_os.h"
void testos() {
#if defined(OI_WIN)
    PRINT("system",TEST(1),"Windows");
#elif defined(OI_LINUX)
    PRINT("system",TEST(1),"Linux");
#elif defined(OI_MAC)
    PRINT("system",TEST(1),"Macintosh");
#elif defined(OI_FreeBSD)
    PRINT("system",TEST(1),"FreeBSD");
#elif defined(OI_UNKOWN_OS)
    PRINT("system",TEST(0),"Unknown");
#else
    PRINT("system",TEST(0),"None");
#endif

#if defined(OI_MSVC)
    PRINT("compiler",TEST(1),"Visual Studio");
#elif defined(OI_GCC)
    PRINT("compiler",TEST(1),"GCC");
#else
    PRINT("compiler",TEST(0),"Unknown");
#endif
    
#if defined(OI_C)
    PRINT("compiler",TEST(1),"C");
#elif defined(OI_CPP)
    PRINT("compiler",TEST(1),"C++");
#else
    PRINT("compiler",TEST(0),"None");
#endif
}

#include "oi_types.h"
void testtypes(void) {
    uint8  ui8  = (uint8 )-1;
    uint16 ui16 = (uint16)-1;
    uint32 ui32 = (uint32)-1;
    uint64 ui64 = (uint64)-1;
    int8  i8  = (int8 )(ui8 /2);
    int16 i16 = (int16)(ui16/2);
    int32 i32 = (int32)(ui32/2);
    int64 i64 = (int64)(ui64/2);

    PRINT("int8", TEST(i8 == 0x7f && sizeof(int8) == 1), "size = %-2d max = %d", sizeof(int8), i8);
    PRINT("uint8", TEST(ui8 == 0xff && sizeof(uint8) == 1), "size = %-2d max = %u", sizeof(uint8), ui8);
    PRINT("int16", TEST(i16 == 0x7fff && sizeof(int16) == 2), "size = %-2d max = %d", sizeof(int16), i16);
    PRINT("uint16", TEST(ui16 == 0xffff && sizeof(uint16) == 2), "size = %-2d max = %u", sizeof(uint16), ui16);
    PRINT("int32", TEST(i32 == 0x7fffffff && sizeof(int32) == 4), "size = %-2d max = %d", sizeof(int32), i32);
    PRINT("uint32", TEST(ui32 == 0xffffffff && sizeof(uint32) == 4), "size = %-2d max = %u", sizeof(uint32), ui32);
    PRINT("int64", TEST(i64 == 0x7fffffffffffffffULL && sizeof(int64) == 8), "size = %-2d max = %lld", sizeof(int64), i64);
    PRINT("uint64", TEST(ui64 == 0xffffffffffffffffULL && sizeof(uint64) == 8), "size = %-2d max = %llu", sizeof(uint64), ui64);
        
    PRINT("float32", TEST(sizeof(float32) == 4), "size = %-2d 1/3->%.20f", sizeof(float32), (float32)(1.0/3.0));
    PRINT("float64", TEST(sizeof(float64) == 8), "size = %-2d 1/3->%.20f", sizeof(float64), (float64)(1.0/3.0));
}

#include "oi_pack.h"
void testpack() {
    uint8 data[16];
    uint8 ii[5] = {0x12,0x34,0x56,0x78,0x9A};
    uint8 oo[5];
    uint8 i8 = 0x12, o8 = 0;
    uint16 i16 = 0x1234, o16 = 0;
    uint32 i32 = 0x12345678, o32 = 0;   
    uint64 i64 = 0x123456789abcdef0ULL, o64 = 0;
    float32 if32 = 12.625, of32 = 0;
    float32 onan32 = 0, ozero32 = 0, oinf32 = 0;
    float64 if64 = 12.625, of64 = 0;    
    float64 onan64 = 0, ozero64 = 0, oinf64 = 0;
    uint8  nan[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    uint8 zero[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8 inf32[4] = {0x7f,0x80,0x00,0x00};
    uint8 inf64[8] = {0x7f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00};
    
    memset(data,0,16);                      
    pack(data,ii,sizeof ii);                     
    unpack(data,oo,sizeof oo);                 
    PRINT("pack", TEST(ii[0] == 0x12), "%s -> %s", MEM(0,ii,sizeof ii), MEM(1,data, sizeof ii));
    PRINT("unpack", TEST(ii[sizeof(ii)-1] == oo[sizeof(ii)-1]), "%s <-", MEM(0,oo,sizeof ii));

#define PACKTEST(n)                         \
    memset(data,0,16);                      \
    pack##n(data,i##n);                     \
    o##n = unpack##n(data);                 \
    PRINT("pack"#n, TEST(*(uint8*)data == 0x12), "%s -> %s", MEM(0,&i##n,sizeof i##n), MEM(1,data, sizeof i##n));\
    PRINT("unpack"#n, TEST(i##n == o##n), "%s <-", MEM(0,&o##n,sizeof i##n));

    PACKTEST(8);
    PACKTEST(16);
    PACKTEST(32);
    PACKTEST(64);
    printf("\n");

#undef PACKTEST
#define PACKTEST(n)                                 \
    memset(data,0,16);                              \
    packf##n(data,if##n);                           \
    of##n = unpackf##n(data);                       \
    PRINT("packf"#n, TEST((*(uint8*)data & 0xf0) == 0x40), "%g -> %s", (double)if##n, MEM(0,data, sizeof(float##n)));\
    PRINT("unpackf"#n, TEST((int)of##n == (int)if##n), "%g <-", (double)of##n);\
    ozero##n = unpackf##n(zero);                    \
    onan##n = unpackf##n(nan);                      \
    oinf##n = unpackf##n(inf##n);                   \
    PRINT("",TEST(ozero##n == 0 && onan##n != onan##n && oinf##n == INFINITY), "0->%g nan->%g inf->%g", (double)ozero##n, (double)onan##n, (double)oinf##n);
       
    PACKTEST(32);
    PACKTEST(64);

#undef PACKTEST
}


#include "oi_thread.h"
void testthreadthread(void * a) {
    int err;
    PRINT("", "\n", "I am thread %d", (int)a);
    
    PRINT("yield", "\n", "thread %d yielding", (int)a);
    err = thread_yield();
    PRINT("", TEST(!err), "thread %d yielding err %d", (int)a, err);
    PRINT("sleep", "\n", "thread %d thread_sleep(100)", (int)a);
    err = thread_sleep(1000);
    PRINT("", TEST(((int)a) == 2), "thread %d not terminated", (int)a);
    PRINT("", TEST(!err), "thread %d thread_sleep err %d", (int)a, err);
}

void testthread() {
    thread_t t1,t2;
    int err;
    err = thread_create(&t1,&testthreadthread, (void*)1);
    PRINT("create", TEST(!err), "creating thread %d err %d", 1, err);
    err = thread_create(&t2,&testthreadthread, (void*)2);
    PRINT(""      , TEST(!err), "creating thread %d err %d", 2, err);
    
    thread_sleep(10);
    
    err = thread_terminate(&t1);
    PRINT("terminate", TEST(!err), "terminating thread %d err %d", 1, err);
    err = thread_join(&t2);
    PRINT("join", TEST(!err), "joining thread %d err %d", 2, err);
    PRINT("", TEST(1), "main thread joined");
    err = thread_destroy(&t1);
    PRINT("destroy", TEST(!err), "destroying thread %d err %d", 1, err);
    err = thread_destroy(&t2);
    PRINT("", TEST(!err), "destroying thread %d err %d", 2, err);
}


#include "oi_local.h"
void testlocalthread(void * p) {
    local_t * lp = (local_t*)p;
    int err;
    int temp;
    
    err = local_set(lp,(void*)2);
    PRINT("set", "\n", "setting value to 2");
    temp = (int)local_get(lp);
    PRINT("get", TEST(temp == 2), "thread 2 value is %d", temp);
}

void testlocal() {
    local_t l;
    thread_t t;
    int err;
    int temp;
    
    err = local_create(&l);
    PRINT("create", TEST(!err), "creating local err %d", err);
    err = local_set(&l,(void*)1);
    PRINT("set", "\n", "setting value to 1");
    temp = (int)local_get(&l);
    PRINT("get", TEST(temp == 1), "thread 1 value is %d", temp);
    
    thread_create(&t,&testlocalthread,&l);
    thread_join(&t);
    thread_destroy(&t);
    
    temp = (int)local_get(&l);
    PRINT("get", TEST(temp == 1), "thread 1 value is %d", temp);
    err = local_create(&l);
    PRINT("destroy", TEST(!err), "destroying local err %d", err);
}

#include "oi_mutex.h"
int testmutexdata;

void testmutexthread(void * p) {
    mutex_t * mp = (mutex_t*)p;
    int err;
    
    err = mutex_try_lock(mp);
    PRINT("try_lock", TEST(err), "thread 2 trying mutex err %d", err);
    err = mutex_lock(mp);
    PRINT("lock", TEST(!err), "thread 2 locking mutex err %d", err);
    
    testmutexdata++;
    PRINT("", TEST(testmutexdata == 2), "data access 2 -> %d", testmutexdata);
    
    err = mutex_unlock(mp);
    PRINT("unlock", TEST(!err), "thread 2 unlocking mutex err %d", err);
}

void testmutex() {
    thread_t t;
    mutex_t m;
    int err;

    testmutexdata = 0;
    
    err = mutex_create(&m);
    PRINT("create", TEST(!err), "creating mutex err %d", err);
    err = mutex_lock(&m);
    PRINT("lock", TEST(!err), "locking mutex err %d", err);    
    
    thread_create(&t,&testmutexthread,&m);
    thread_sleep(100); printf("\n");
    
    err = mutex_lock(&m);
    PRINT("lock", TEST(!err), "locking mutex err %d", err);
    
    testmutexdata++;
    PRINT("", TEST(testmutexdata == 1), "data access 1 -> %d", testmutexdata);
    
    err = mutex_unlock(&m);
    PRINT("unlock", TEST(!err), "unlocking mutex err %d", err);
    err = mutex_unlock(&m);
    PRINT("unlock", TEST(!err), "unlocking mutex err %d", err);
    
    thread_join(&t);
    thread_destroy(&t);
    err = mutex_destroy(&m);
    PRINT("delete", TEST(!err), "destroying mutex err %d", err);
}



#include "oi_rwlock.h"
void testrwlockthread(void * d) {
    rwlock_t * prw = (rwlock_t*)d;
    int err;

    err = rwlock_try_read_lock(prw);
    PRINT("try_read", TEST(err), "read thread trying lock err %d", err);
    err = rwlock_read_lock(prw);
    PRINT("read_lock", TEST(!err), "read thread locking err %d", err);
    thread_sleep(100); printf("\n");
    
    err = rwlock_read_unlock(prw);
    PRINT("read_unlk", TEST(!err), "read thread unlocking err %d", err);
}

void testrwlock() {
    thread_t t1,t2;
    rwlock_t rw;
    int err;

    err = rwlock_create(&rw);
    PRINT("create", TEST(!err), "creating rwlock err %d", err);
    err = rwlock_write_lock(&rw);
    PRINT("write_lock", TEST(!err), "write thread locking err %d", err);
    
    thread_create(&t1,&testrwlockthread,&rw);
    thread_create(&t2,&testrwlockthread,&rw);

    thread_sleep(50); printf("\n");

    err = rwlock_write_unlock(&rw);
    PRINT("write_unlk", TEST(!err), "write thread unlocking err %d", err);

    thread_sleep(50); printf("\n");

    err = rwlock_try_write_lock(&rw);
    PRINT("try_write", TEST(err), "write thread trying lock err %d", err);
    err = rwlock_write_lock(&rw);
    PRINT("write_lock", TEST(!err), "write thread locking err %d", err);
    err = rwlock_write_unlock(&rw);
    PRINT("write_unlk", TEST(!err), "write thread unlocking err %d", err);
    
    thread_join(&t1);
    thread_join(&t2);
    thread_destroy(&t1);
    thread_destroy(&t2);
    
    err = rwlock_destroy(&rw);
    PRINT("destroy", TEST(!err), "destroying rwlock err %d", err);
}

#include "oi_cond.h"
cond_t * condpc;
mutex_t * condpm;
int condmax;
int condcount;
void testcondthread(void * timed) {
    int err;

    mutex_lock(condpm);
    while (1) {
		err = timed ? cond_timed_wait(condpc,condpm,100) : cond_wait(condpc,condpm);
        if (condcount < 0) break;
        PRINT(timed ? "timed_wait" : "wait", TEST(!err), "woken thread err %d", err);
        condcount++;
        PRINT("", TEST(condcount <= condmax), "total signals = %d", condcount);
    }
    
    PRINT("", TEST(!err ^ (int)timed), "woken thread err %d", err);
    mutex_unlock(condpm);
}


void testcond() {
    thread_t t1,t2,t3;
    cond_t c;
    mutex_t m;
    int err;

    mutex_create(&m);
    condpm = &m;
    err = cond_create(&c);
	condpc = &c;
    PRINT("create", TEST(!err), "creating cond err %d", err);

    thread_create(&t1,&testcondthread,(void*)0);
    thread_create(&t2,&testcondthread,(void*)1);
    thread_create(&t3,&testcondthread,(void*)1);

    mutex_lock(&m);
    condmax = 1;
    condcount = 0;
    err = cond_signal_one(&c);
    PRINT("signal_one", TEST(!err), "signaling one err %d", err);
    mutex_unlock(&m);

    thread_sleep(50); printf("\n");

    mutex_lock(&m);
    condmax = 3;
    condcount = 0;
    err = cond_signal_all(&c);
    PRINT("signal_all", TEST(!err), "signaling all err %d", err);
    mutex_unlock(&m);

    thread_sleep(50); printf("\n");

    mutex_lock(&m);
    condcount = -1;
    PRINT("time_out", "\n", "waiting for timeout...");
    mutex_unlock(&m);

    thread_join(&t2);
    thread_destroy(&t2);
    thread_join(&t3);
    thread_destroy(&t3);

    err = cond_signal_all(&c);
    PRINT("", TEST(!err), "terminating and signalling err %d", err);
	thread_join(&t1);
	thread_destroy(&t1);

    err = cond_destroy(&c);
    PRINT("destroy", TEST(!err), "destroying cond err %d", err);
}

#include "oi_time.h"
void testtime() {
    int err;
    uint64 b,a;
    
    b=millis();
    PRINT("millis", "\n", "before -> %llu", b);
	err=thread_sleep(1500);
    PRINT("sleep", TEST(!err), "thread_sleep(1500) err %d", err);
	err=thread_sleep(1000);
    PRINT("", TEST(!err), "thread_sleep(1000) err %d", err);
	err=thread_sleep(500);
    PRINT("", TEST(!err), "thread_sleep(500)  err %d", err);
    a = millis();
    PRINT("", TEST((a-b)/100 == 30), "after  -> %llu  diff = %llu", a, a-b);    
}

#include "oi_address.h"
void testaddress() {
    address_t a;
    int err;
    uint8 * to;
    size_t len;
    uint16 oport;
    char buff[2][41];
    int time;

    uint8 ta4[] = {8,8,8,8};
    uint8 ta6[] = {0x20,0x01,0x48,0x60,0x48,0x60,0,0,0,0,0,0,0,0,0x88,0x88};
    uint16 port = 12345; 

#define ADDTESTWITH(test)   \
    to = (uint8*)address_address(&a,&len); \
    PRINT("address", test, " addr->%s", MEM(0,to,len));\
    oport = address_port(&a);   \
    PRINT("port", TEST(oport == port), " port->%d",oport);\
    time = millis();                \
    err = address_name(&a,buff[0],sizeof buff[0],1);\
    err|= address_name(&a,buff[1],sizeof buff[1],0);\
    PRINT("name", "\n", " with->%s", buff[0]);      \
    PRINT("", "\n", " wout->%s", buff[1]);              \
    PRINT("", TEST(!err), "get name err %d time %d", err, millis()-time);

#define ADDTESTWOUT(test)   \
    to = (uint8*)address_address(&a,&len); \
    PRINT("address", test, " addr->%s", MEM(0,to,len));\
    oport = address_port(&a);   \
    PRINT("port", TEST(oport == port), " port->%d",oport);\
    err = address_name(&a,buff[1],sizeof buff[1],0);\
    PRINT("name", "\n", " wout->%s", buff[1]);


    err = address_from_ipv4(&a,&ta4,port);
    PRINT("from_ipv4", "\n", "%s:%d", MEM(0,ta4,4), port);
    PRINT("", TEST(!err), "address from ipv4 err %d", err);
    ADDTESTWITH(TEST(to[0]==ta4[0]));
    printf("\n");

    err = address_from_ipv6(&a,&ta6,port);
    PRINT("from_ipv6", "\n", "%s:%d", MEM(0,ta6,16), port);
    PRINT("", TEST(!err), "address from ipv6 err %d", err);
    ADDTESTWITH(TEST(to[0]==ta6[0]));
    printf("\n");

    err = address_from_name(&a,"192.0.2.0",port,0);
    PRINT("from_name", "\n", "\"192.0.2.0\"");
    PRINT("", TEST(!err), "address from name err %d", err);
    ADDTESTWOUT(TEST(to[0]==0xc0));
    printf("\n");

    err = address_from_name(&a,"2001:db8::",port,0);
    PRINT("from_name", "\n", "\"2001:db8::\"");
    PRINT("", TEST(!err), "address from name err %d", err);
    ADDTESTWOUT(TEST(to[0]==0x20));
    printf("\n");

    err = address_from_name(&a,"www.google.com",port,1);
    PRINT("from_name", "\n", "\"www.google.com\"");
    PRINT("", TEST(!err), "address from name err %d", err);
    ADDTESTWITH("\n");
    printf("\n");

    err = address_loopback(&a,port);
    PRINT("loopback", TEST(!err), "address loopback err %d", err);
    ADDTESTWITH(TEST(to[0]==127 || to[15]==1));
    printf("\n");
    
    err = address_host(&a,port);
    PRINT("local", TEST(!err), "address local err %d", err);
    ADDTESTWITH("\n");
}

#define TESTF(file) if (!all || !strcmp(argv[argc-1],#file) || !strcmp(argv[argc-1],"oi_"#file)) {printf("\noi_"#file" :\n"); test##file();}

int main(int argc, char ** argv) {
    int all = argc>1; 
    
    for (;argc > 0; argc--) {       
        TESTF(os);
        TESTF(types);
        TESTF(pack);
        TESTF(thread);
        TESTF(local);
        TESTF(mutex);
        TESTF(rwlock);
        TESTF(cond);
        TESTF(time);
        TESTF(address);
    }

    if (rrr) printf("\noi has failed a test on this system.\nchanges are necessary for oi to work.\nFAILED!\n\n");
    else printf("\noi is functional on this system.\nsuccess!\n\n");

#ifdef WAIT    
    printf("Press enter to continue\n");
    getchar();
#endif
    
    return rrr;
}


