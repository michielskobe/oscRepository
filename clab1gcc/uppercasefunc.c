#include <stdio.h>
#include "uppercasefunc.h"

void toUppercase(char * who) {
	int i;
	for (i = 0; i < sizeof(who); i++) {
	who[i] = who[i] - 32;
	}	
	
	return;
}
