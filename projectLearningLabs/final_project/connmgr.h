#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

typedef struct conn_param {
    int max_conn;
    int port;
    sbuffer_t *buffer;
    int write_end;
} conn_param_t;

void *connection_manager(void *arg);
void *connection_routine(void *arg);


