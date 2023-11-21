#include <stdio.h>
#include "sizefunc.h"

void myPrintSize() {
	
	printf("Size of int: %ld\n", sizeof(int));
	printf("Size of float: %ld\n", sizeof(float));
	printf("Size of double: %ld\n", sizeof(double));
	printf("Size of void: %ld\n", sizeof(void));
	printf("Size of pointer: %ld\n", sizeof(char*));
	printf("Size of short: %ld\n", sizeof(short));
	printf("Size of long: %ld\n", sizeof(long));
	
	return;
}
