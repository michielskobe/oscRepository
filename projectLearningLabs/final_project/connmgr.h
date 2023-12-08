#include "sbuffer.h"

typedef struct conn_param {
    int max_conn;
    int port;
    sbuffer_t *buffer;
} conn_param_t;

void *connection_manager(void *arg);
void *tcp_handler(void *arg);


