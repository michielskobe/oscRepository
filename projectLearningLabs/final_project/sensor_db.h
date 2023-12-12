/**
 * \author Kobe Michiels
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include "config.h"
#include "sbuffer.h"

typedef struct store_param {
    sbuffer_t *buffer;
    int write_end;
} store_param_t;


void *storage_manager(void *arg);


#endif /* _SENSOR_DB_H_ */