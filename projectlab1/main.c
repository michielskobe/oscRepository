#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lib/dplist.h"
#include "datamgr.h"
#include <time.h>

int main(){
    printf("Hello World\n");

    FILE * map = fopen("room_sensor.map", "r");
    FILE * data = fopen("sensor_data", "rb");

    if(map == NULL) return -1;
    if(data == NULL) return -1;

    datamgr_parse_sensor_files(map, data);

    printf("Total sensors: %d\n", datamgr_get_total_sensors());
    datamgr_free();


    fclose(map);
    fclose(data);
}