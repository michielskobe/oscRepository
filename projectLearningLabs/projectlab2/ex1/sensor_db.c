/**
 * \author Kobe Michiels
 */

#include "sensor_db.h"
#include <stdio.h>
#include <stdbool.h>

FILE * open_db(char * filename, bool append) {
    FILE *f = fopen(filename, append ? "a" : "w");
    if (f == NULL) {
        fprintf(stderr, "Error: Failed to open file.");
        exit(EXIT_FAILURE);
    }
    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    int insert_value = fprintf(f, "%d,%f,%ld\n", id, value, ts);
    if (insert_value < 0) {
        fprintf(stderr, "Error: Failed to insert data.");
        exit(EXIT_FAILURE);
    }
    return insert_value;
}

int close_db(FILE * f){
    int close_status = fclose(f);
    if (close_status != 0) {
        fprintf(stderr, "Error: Failed to close file.");
        exit(EXIT_FAILURE);
    }
    return close_status;
}