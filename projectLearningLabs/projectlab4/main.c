/**
 * \author Kobe Michiels
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sbuffer.h"

#define WRITE_WAIT_TIME 10000
#define READ_WAIT_TIME 25000

void *buffer_write_handler();
void *buffer_read_handler();

sbuffer_t *buffer;

int main() {

    if (sbuffer_init(&buffer) == SBUFFER_FAILURE){
        fprintf(stderr, "Error: Initialization of shared buffer failed.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t writer_thread, reader_thread1, reader_thread2;

    if (pthread_create(&writer_thread, NULL, buffer_write_handler, NULL) != 0){
        fprintf(stderr, "Error: Creation of writer thread failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader_thread1, NULL, buffer_read_handler, NULL) != 0){
        fprintf(stderr, "Error: Creation of first reader thread failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader_thread2, NULL, buffer_read_handler, NULL) != 0){
        fprintf(stderr, "Error: Creation of second reader thread failed.\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(writer_thread, NULL) != 0) {
        fprintf(stderr, "Error: Joining writer thread failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(reader_thread1, NULL) != 0) {
        fprintf(stderr, "Error: Joining first reader thread failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_join(reader_thread2, NULL) != 0) {
        fprintf(stderr, "Error: Joining second reader thread failed.\n");
        exit(EXIT_FAILURE);
    }

    if (sbuffer_free(&buffer) == SBUFFER_FAILURE){
        fprintf(stderr, "Error: Freeing of allocated shared buffer resources failed.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *buffer_write_handler() {
    uint16_t sensor_id;
    double temperature;
    time_t timestamp;
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    FILE *fp_sensor_data = fopen("sensor_data", "rb");

    while (fread(&sensor_id, sizeof(sensor_id), 1, fp_sensor_data) != 0) {
        fread(&temperature, sizeof(temperature), 1, fp_sensor_data);
        fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data);
        data->id = sensor_id;
        data->value = temperature;
        data->ts = timestamp;
        if (sbuffer_insert(buffer, data) == SBUFFER_FAILURE){
            fprintf(stderr, "Error: Data insertion in shared buffer failed.\n");
            exit(EXIT_FAILURE);
        }
        usleep(WRITE_WAIT_TIME);
    }
    data->id = 0;
    data->value = 0;
    data->ts = 0;
    if (sbuffer_insert(buffer, data) == SBUFFER_FAILURE){
        fprintf(stderr, "Error: End-of-stream marker insertion in shared buffer failed.\n");
        exit(EXIT_FAILURE);
    }
    free(data);
    fclose(fp_sensor_data);
    pthread_exit(0);
}

void *buffer_read_handler() {
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    while (1) {
        if (sbuffer_remove(buffer, data) == SBUFFER_FAILURE){
            fprintf(stderr, "Error: Data removal from shared buffer failed.\n");
            exit(EXIT_FAILURE);
        }
        if (data->id == 0){
            break;
        }
        FILE *fp_sensor_data_csv = fopen("sensor_data_out.csv", "a");
        fprintf(fp_sensor_data_csv, "%d,%f,%ld\n", data->id, data->value, data->ts);
        fclose(fp_sensor_data_csv);
        usleep(READ_WAIT_TIME);
    }
    free(data);
    pthread_exit(0);
}