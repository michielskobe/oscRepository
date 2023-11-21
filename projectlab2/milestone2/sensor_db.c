/**
 * \author Kobe Michiels
 */

#include "sensor_db.h"
#include <stdio.h>
#include <stdbool.h>
#include "logger.h"
#include <string.h>
#include <unistd.h>

#define SIZE 128
int fd_write;
char write_msg[SIZE];

FILE * open_db(char * filename, bool append) {
    fd_write = create_log_process();
    // open csv file
    FILE *f = fopen(filename, append? "a" : "w");
    // create log message based on open status
    if (f == NULL) {
        sprintf(write_msg, "%s", "Error: Failed to open file");
        write(fd_write, write_msg, SIZE);
    } else {
        sprintf(write_msg, "%s", "Data file opened");
        write(fd_write, write_msg, SIZE);
    }
    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        sprintf(write_msg, "%s", "Error: Failed to insert into file - file is NULL");
        write(fd_write, write_msg, SIZE);
        // terminate process to prevent orphan child because of insert in non-existing file
        sprintf(write_msg, "%s", "Terminate process");
        write(fd_write, write_msg, SIZE);
        exit(EXIT_FAILURE);
    } else {
        // insert data in csv file
        int insert_status = fprintf(f, "%d,%f,%ld\n", id, value, ts);
        // create log message based on insert status
        if (insert_status < 0) {
            sprintf(write_msg, "%s", "Error: Failed to insert data");
            write(fd_write, write_msg, SIZE);
        } else {
            sprintf(write_msg, "%s", "Data inserted");
            write(fd_write, write_msg, SIZE);
        }
        return insert_status;
    }
}

int close_db(FILE * f){
    if (f == NULL) {
        sprintf(write_msg, "%s", "Error: Failed to close file - file is NULL");
        write(fd_write, write_msg, SIZE);
        // terminate process to prevent orphan child because of attempt to close non-existing file
        sprintf(write_msg, "%s", "Terminate process");
        write(fd_write, write_msg, SIZE);
        exit(EXIT_FAILURE);
    }
    else {
        // close csv file
        int close_status = fclose(f);
        sprintf(write_msg, "%s", "Data file closed");
        write(fd_write, write_msg, SIZE);
        sprintf(write_msg, "%s", "Terminate process");
        write(fd_write, write_msg, SIZE);
        return close_status;
    }
}