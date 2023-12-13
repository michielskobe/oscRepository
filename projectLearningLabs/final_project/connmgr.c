/**
 * \author Kobe Michiels
 */

#include "connmgr.h"

int fd_connmgr;
char log_msg_connmgr[SIZE];
sbuffer_t *buffer_connmgr;

void *connection_manager(void *arg) {
    conn_thread_arg_t *conn_thread_arg = arg;

    int MAX_CONN = conn_thread_arg->max_conn;
    int PORT = conn_thread_arg->port;
    buffer_connmgr = conn_thread_arg->buffer;
    fd_connmgr = conn_thread_arg->fd;

    tcpsock_t *server, *client[MAX_CONN];
    pthread_t tid[MAX_CONN];
    int conn_counter = 0;

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

    // insert end-of-stream marker
    sensor_data_t *end_of_stream_marker = malloc(sizeof(sensor_data_t));
    end_of_stream_marker->id=0;
    sbuffer_insert(buffer_connmgr, end_of_stream_marker);
    free(end_of_stream_marker);
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
        if (result == TCP_TIMEOUT) break;
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        if (result == TCP_TIMEOUT) break;
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        if (result == TCP_TIMEOUT) break;
        if (first_data_packet){
            sprintf(log_msg_connmgr, "Sensor node %" PRIu16 " has opened a new connection", data.id);
            write(fd_connmgr, log_msg_connmgr, SIZE);
            first_data_packet = false;
        }
        if ((result == TCP_NO_ERROR) && bytes) {
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                   (long int) data.ts);
            sbuffer_insert(buffer_connmgr, &data);
        }
    } while (result == TCP_NO_ERROR);
    if (result == TCP_TIMEOUT) {
        printf("Peer has closed connection (Connection time-out)\n");
        sprintf(log_msg_connmgr, "Connection time-out: Sensor node %" PRIu16 " has closed the connection", data.id);
        write(fd_connmgr, log_msg_connmgr, SIZE);
    }
    else if (result == TCP_CONNECTION_CLOSED) {
        printf("Peer has closed connection\n");
        sprintf(log_msg_connmgr, "Sensor node %" PRIu16 " has closed the connection", data.id);
        write(fd_connmgr, log_msg_connmgr, SIZE);
    }
    else {
        printf("Error occurred on connection to peer\n");
        sprintf(log_msg_connmgr, "Error occurred on connection to sensor node %" PRIu16 "", data.id);
        write(fd_connmgr, log_msg_connmgr, SIZE);
    }
    tcp_close(&client);
    pthread_exit(0);
}