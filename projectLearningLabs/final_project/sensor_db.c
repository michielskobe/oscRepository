/**
 * \author Kobe Michiels
 */

#include "sensor_db.h"

int stor_log_fd;
char stor_log_msg[SIZE];
sbuffer_t *stor_buffer;

void *storage_manager(void *arg) {
    store_param_t *stor_param = arg;
    stor_buffer = stor_param->buffer;
    stor_log_fd = stor_param->write_end;
    // create new data.csv file
    fclose(fopen("data.csv", "w"));
    sprintf(stor_log_msg, "A new data.csv file has been created.");
    write(stor_log_fd, stor_log_msg, SIZE);

    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    while(sbuffer_remove(stor_buffer, data) != SBUFFER_NO_DATA){
        FILE *data_file = fopen("data.csv", "a");
        fprintf(data_file, "%d,%f,%ld\n", data->id, data->value, data->ts);
        fclose(data_file);
        sprintf(stor_log_msg, "Data insertion from sensor %" PRIu16 " succeeded", data->id);
        write(stor_log_fd, stor_log_msg, SIZE);
    }
    sprintf(stor_log_msg, "The data.csv file has been closed.");
    write(stor_log_fd, stor_log_msg, SIZE);
    return 0;
}