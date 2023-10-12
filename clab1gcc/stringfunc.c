#include <stdio.h>
#include "stringfunc.h"

void myStringFunc() {
	
	return;
}

char * toUppercase(char * who) {
	int i;
	for (i = 0; i < sizeof(who); i++) {
	if (who[i] != 0) {
		who[i] = who[i] - 32;
		}
	}	
	
	return who;
}
