/**
 * \author Kobe Michiels
 */

#ifndef DATAMGR_H_
#define DATAMGR_H_

#include <assert.h>
#include <unistd.h>
#include <inttypes.h>

#include "config.h"
#include "lib/dplist.h"
#include "sbuffer.h"

typedef struct data_thread_arg {
    sbuffer_t *buffer;
    int fd;
} data_thread_arg_t;

void *data_manager(void *arg);
dplist_node_t *get_sensor_with_id(sensor_id_t sensor_id);

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