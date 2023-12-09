/**
 * \author Kobe Michiels
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "lib/dplist.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 25
//#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 15
//#error SET_MIN_TEMP not set
#endif

#include "sbuffer.h"

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)    do { \
                      if (condition) { \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE); \
                      } \
                    } while(0) \

typedef struct data_param {
    sbuffer_t *buffer;
} data_param_t;

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them.
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void *datamgr_parse_sensor_files(void *arg);

/**
 * This method should be called to clean up the datamgr, and to free all used memory.
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();



dplist_node_t *datamgr_get_sensor_with_id(sensor_id_t sensor_id);

#endif  //DATAMGR_H_