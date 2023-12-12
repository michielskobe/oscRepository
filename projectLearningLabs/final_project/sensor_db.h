/**
 * \author Kobe Michiels
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include "config.h"
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "sbuffer.h"

typedef struct stor_thread_arg {
    sbuffer_t *buffer;
    int fd;
} stor_thread_arg_t;


void *storage_manager(void *arg);


#endif /* _SENSOR_DB_H_ */