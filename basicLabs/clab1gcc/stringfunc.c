#include <stdio.h>
#include "stringfunc.h"
#include "string.h"

char * toUppercase(char * who) {
	int i;
	static char upper[sizeof(who)];
	strcpy(upper, who);
	for (i = 0; i < sizeof(who); i++) {
		if (upper[i] != 0) {
			upper[i] = upper[i] - 32;
		}
	}
	return upper;
}
