#include <stdio.h>
#include "ud_threads.h"
#include "ud_time.h"

void test(void * u) {
	while (1) {
		printf("t%d : %d\n",u,millis());
	}
}


int main() {
	thread_t a,b,c,d;
	thread_create(a,&test,0);
//	thread_create(b,&test,1);
//	thread_create(c,&test,2);
//	thread_create(d,&test,3);

	test(4);
}

