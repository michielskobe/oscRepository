/**
 * \author Kobe Michiels
 */

#include "sensor_db.h"

int fd_stormgr;
char log_msg_stormgr[SIZE];
sbuffer_t *buffer_stormgr;

void *storage_manager(void *arg) {
    stor_thread_arg_t *stor_thread_arg = arg;
    buffer_stormgr = stor_thread_arg->buffer;
    fd_stormgr = stor_thread_arg->fd;

    // create new data.csv file
    FILE *data_file = fopen("data.csv", "w");
    // log data.csv file creation
    sprintf(log_msg_stormgr, "A new data.csv file has been created");
    write(fd_stormgr, log_msg_stormgr, SIZE);

    // insert buffer data in data.csv file
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while(sbuffer_remove(buffer_stormgr, data) != SBUFFER_NO_DATA){
        fprintf(data_file, "%d,%f,%ld\n", data->id, data->value, data->ts);
        fflush(data_file);
        sprintf(log_msg_stormgr, "Data insertion from sensor %" PRIu16 " succeeded", data->id);
        write(fd_stormgr, log_msg_stormgr, SIZE);
    }
    // close data.csv file
    fclose(data_file);
    sprintf(log_msg_stormgr, "The data.csv file has been closed");
    write(fd_stormgr, log_msg_stormgr, SIZE);
    free(data);
    pthread_exit(0);
}