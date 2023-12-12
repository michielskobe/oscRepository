/**
 * \author Kobe Michiels
 */

#include "connmgr.h"

int conn_log_fd;
char conn_log_msg[SIZE];
sbuffer_t *conn_buffer;

void *connection_manager(void *arg) {
    conn_param_t *conn_param = arg;

    int MAX_CONN = conn_param->max_conn;
    int PORT = conn_param->port;
    conn_buffer = conn_param->buffer;
    conn_log_fd = conn_param->write_end;

    tcpsock_t *server, *client[MAX_CONN];
    int conn_counter = 0;

    pthread_t tid[MAX_CONN];

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    do {
        if (tcp_wait_for_connection(server, &client[conn_counter]) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Incoming client connection\n");
        pthread_create(&tid[conn_counter], NULL, connection_routine, client[conn_counter]);
        conn_counter++;
    } while (conn_counter < MAX_CONN);
    for (int i = 0; i < MAX_CONN; i++)
        pthread_join(tid[i], NULL);
    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");
    pthread_exit(0);
}

void *connection_routine(void *arg) {
    tcpsock_t *client = (tcpsock_t*)arg;
    sensor_data_t data;
    int bytes, result;
    bool first_data_packet = true;
    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        if (first_data_packet){
            sprintf(conn_log_msg, "Sensor node %" PRIu16 " has opened a new connection", data.id);
            write(conn_log_fd, conn_log_msg, SIZE);
            first_data_packet = false;
        }
        if ((result == TCP_NO_ERROR) && bytes) {
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                   (long int) data.ts);
            sbuffer_insert(conn_buffer, &data);
        }
    } while (result == TCP_NO_ERROR);
    if (result == TCP_CONNECTION_CLOSED) {
        printf("Peer has closed connection\n");
        sprintf(conn_log_msg, "Sensor node %" PRIu16 " has closed the connection", data.id);
        write(conn_log_fd, conn_log_msg, SIZE);
    }
    else
        printf("Error occured on connection to peer\n");
    tcp_close(&client);
    pthread_exit(0);
}