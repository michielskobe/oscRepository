/**
 * \author Kobe Michiels
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>

#include "config.h"
#include "lib/dplist.h"
#include "sbuffer.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#define SET_MAX_TEMP 10
//#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
#define SET_MIN_TEMP 5
//#error SET_MIN_TEMP not set
#endif

typedef struct data_param {
    sbuffer_t *buffer;
    int write_end;
} data_param_t;

void *data_manager(void *arg);
dplist_node_t *datamgr_get_sensor_with_id(sensor_id_t sensor_id);

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)    do { \
                      if (condition) { \
                        printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
                        exit(EXIT_FAILURE); \
                      } \
                    } while(0)                \

#endif  // DATAMGR_H_