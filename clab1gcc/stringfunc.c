#include <stdio.h>
#include "stringfunc.h"
#include "string.h"

void myStringFunc() {
	
	return;
}

void toUppercase(char * who, char * where) {
	int i;
	char upper[sizeof(who)];
	for (i = 0; i < sizeof(who); i++) {
		if (who[i] != 0) {
			upper[i] = who[i] - 32;
		}
	}
	strcpy(where, upper);	
	return;
}
