/**
 * \author Kobe Michiels
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sbuffer.h"


void *buffer_write_handler(void *arg);
void *buffer_read_handler(void *arg);

sbuffer_t *buffer;

int main() {

    sbuffer_init(&buffer);

    pthread_t writer_thread, reader_thread1, reader_thread2;

    pthread_create(&writer_thread, NULL, buffer_write_handler, NULL);
    pthread_create(&reader_thread1, NULL, buffer_read_handler, NULL);
    pthread_create(&reader_thread2, NULL, buffer_read_handler, NULL);

    pthread_join(writer_thread, NULL);
    pthread_join(reader_thread1, NULL);
    pthread_join(reader_thread2, NULL);

    sbuffer_free(&buffer);

    return 0;
}

void *buffer_write_handler(void *arg) {
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    uint16_t sensor_id;
    double temperature;
    time_t timestamp;
    FILE *fp_sensor_data = fopen("sensor_data", "rb");
    while (fread(&sensor_id, sizeof(sensor_id), 1, fp_sensor_data) != 0) {
        fread(&temperature, sizeof(temperature), 1, fp_sensor_data);
        fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data);
        data->id = sensor_id;
        data->value = temperature;
        data->ts = timestamp;
        sbuffer_insert(buffer, data);
        usleep(10000); //wait 10ms after inserting a measurement
    }
    data->id = 0;
    data->value = 0;
    data->ts = 0;
    sbuffer_insert(buffer, data);
    usleep(10000); //wait 10ms after inserting a measurement
    fclose(fp_sensor_data);
    free(data);
    pthread_exit(0);
}

void *buffer_read_handler(void *arg) {
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    while (1) {
        sbuffer_remove(buffer, data);
        if (data->id == 0){
            break;
        }
        FILE *fp_sensor_data_csv = fopen("sensor_data_out.csv", "a");
        fprintf(fp_sensor_data_csv, "%d,%f,%ld\n", data->id, data->value, data->ts);
        fclose(fp_sensor_data_csv);
        usleep(25); //wait 25ms after writing a measurement
    }
    free(data);
    pthread_exit(0);
}