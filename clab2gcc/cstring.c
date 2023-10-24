//
// Created by syssoft on 13/10/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *str;
    int len;
} CString;

CString *Init_CString(char *str) {
    CString *p = (CString*)malloc(sizeof(CString));
    p->len = strlen(str);
    char *ptr = malloc(sizeof(str));
    strncpy(ptr, str, strlen(str)+1);
    p->str = ptr;
    return p;
}

void Delete_CString(CString *p) {
    free(p->str);
    free(p);
}

// Removes the last character of a CString and returns it.
//
char Chomp(CString *cstring) {
    char lastchar = *(cstring->str + cstring->len - 1);
    // Shorten the string by one
    *(cstring->str + cstring->len - 1) = '\0';
    cstring->len = strlen(cstring->str);

    return lastchar;
}

// Appends a char * to a CString //
CString *Append_Chars_To_CString(CString *p, char *str) {
    char *newstr = malloc(p->len + strlen(str));
    int newlen = p->len + strlen(str) + 1;
    p->len = p->len + strlen(str);

    // Create the new string to replace p->str
    snprintf(newstr, newlen, "%s%s", p->str, str);
    // Free old string and make CString point to the new string
    free(p->str);
    p->str = newstr;

    return p;
}

int main(void) {
    CString *mystr;
    char c;

    mystr = Init_CString("Hello!");
    printf("Init:\n str: `%s' len: %d\n", mystr->str, mystr->len);
    c = Chomp(mystr);
    printf("Chomp '%c':\n str:`%s' len: %d\n", c, mystr->str, mystr->len);
    mystr = Append_Chars_To_CString(mystr, " world!");
    printf("Append:\n str: `%s' len: %d\n", mystr->str, mystr->len);

    Delete_CString(mystr);

    return 0;
}


