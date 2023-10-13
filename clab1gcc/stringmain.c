#include "stringfunc.h"
#include <stdio.h>
#include "string.h"

int main (int argc, char **argv) {
	int MAX = 20;
	char first[MAX];
	char second[MAX];
	char name[MAX];
	char str[MAX];

	printf("Enter first name: ");
   	scanf("%s", first);
   	
   	printf("Enter second name: ");
   	scanf("%s", second);
   	
   	toUppercase(second, str);
   	printf("strcmp of second and str: %i\n", strcmp(second, str));
   	
   	strcat(strcat(name, first), second);
   	printf("name: %s\n",name);
   	
   	int year;
   	printf("Enter birth year: ");
   	scanf("%d", &year);

	snprintf(name, 100, "%s %s %d",first, second, year);
	printf("name: %s\n",name);
	
	sscanf( name, "%s %s %d", first, second, &year);
	printf("first: %s; second: %s; year: %d\n", first, second, year);
	return 0;
}
