#include "sbuffer.h"

typedef struct data_param {
    sbuffer_t *buffer;
} data_param_t;

void *data_manager(void *arg);
void *tcp_handler(void *arg);