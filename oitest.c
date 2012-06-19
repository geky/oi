#include <stdio.h>

char arrrr[36];
char rrr = 0;
#define BEGIN(xx) printf("\n"#xx" :\n")
#define PRINT(tt,ss,xx...) {sprintf(arrrr, ss, ##xx); printf("\t%-8s : %-36s : ", #tt, arrrr);}
#define TEST(yy) {if (yy) printf("success\n"); else {printf("FAILURE!\n"); rrr=1;}}

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
	PRINT(int8  ,"size->%d max->%d", sizeof(int8), i8);
	TEST(i8 == 0x7f && sizeof(int8) == 1);
	PRINT(uint8 ,"size->%d max->%u", sizeof(uint8), ui8);
	TEST(ui8 == 0xff && sizeof(uint8) == 1);
	PRINT(int16 ,"size->%d max->%d", sizeof(int16), i16);    
	TEST(i16 == 0x7fff && sizeof(int16) == 2);
	PRINT(uint16,"size->%d max->%u", sizeof(uint16), ui16);   
	TEST(ui16 == 0xffff && sizeof(uint16) == 2);
	PRINT(int32 ,"size->%d max->%d", sizeof(int32), i32);    
	TEST(i32 == 0x7fffffff && sizeof(int32) == 4);
	PRINT(uint32,"size->%d max->%u", sizeof(uint32), ui32);   
	TEST(ui32 == 0xffffffff && sizeof(uint32) == 4);
	PRINT(int64 ,"size->%d max->%lld", sizeof(int64), i64);  
	TEST(i64 == 0x7fffffffffffffffULL && sizeof(int64) == 8);
	PRINT(uint64,"size->%d max->%llu", sizeof(uint64), ui64); 
	TEST(ui64 == 0xffffffffffffffffULL && sizeof(uint64) == 8);
	PRINT(float32,"size->%d 1/3->%.20f", sizeof(float32), (float32)(1.0/3.0));
	TEST(sizeof(float32) == 4);
	PRINT(float64,"size->%d 1/3->%.20f", sizeof(float64), (float64)(1.0/3.0));
	TEST(sizeof(float64) == 8);
}

int main() {
	printf("Checking oi's operability\n");

	testtypes();
	
	printf("\n");
	return 0;
}


