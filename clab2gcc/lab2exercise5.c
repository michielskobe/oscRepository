#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQ 2
#define MIN_TEMP (-10)
#define MAX_TEMP 35

int main( void ) {
    int temperature;
    srand(time(NULL));
    for (;;) {
        temperature = (rand() % (MAX_TEMP-MIN_TEMP) + MIN_TEMP);
        struct tm* local;
        time_t t = time(NULL);
        local = localtime(&t);
        char* time = asctime(local);
        printf("Temperature = %d°C @%s",temperature,time);
        fflush(stdout);
        sleep(FREQ);
    }
  return 0;
}