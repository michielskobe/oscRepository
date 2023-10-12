#include "stringfunc.h"
#include "string.h"

int main (int argc, char **argv) {
	int MAX = 20;
	char first[MAX];
	char second[MAX];
	char name[MAX];
	char str[MAX];
	
	printf("Enter first name: ");
   	scanf("%s", first);
   	printf("(first: %s)\n",first);
   	
   	printf("Enter second name: ");
   	scanf("%s", second);
   	printf("(second: %s)\n",second);
   	
   	strcpy(str, toUppercase(second));
   	printf("(str: %s)\n",str);
   	
   	printf("strcmp of second and str: %i\n", strcmp(second, str));
   

	
	return 0;
}
