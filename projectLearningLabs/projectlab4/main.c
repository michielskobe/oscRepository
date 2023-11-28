/**
 * \author Kobe Michiels
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sbuffer.h"
#include <stdbool.h>
#define NUM_WRITE_THREADS 1
#define NUM_READ_THREADS 2

typedef struct sbuffer_node {
    struct sbuffer_node *next;
    sensor_data_t data;
} sbuffer_node_t;

struct sbuffer {
    sbuffer_node_t *head;
    sbuffer_node_t *tail;
};

void *buffer_write_handler(void *arg);
void *buffer_read_handler(void *arg);

sbuffer_t *buffer;
pthread_mutex_t mutex;

int main(int argc, char *argv[]) {

    int buffer_status = sbuffer_init(&buffer);
    if (buffer_status != SBUFFER_SUCCESS){
        fprintf(stderr, "Buffer init failed");
    }

    pthread_t tid_w[NUM_WRITE_THREADS];
    pthread_t tid_r[NUM_READ_THREADS];

    pthread_mutex_init(&mutex, NULL);

    int i,j = 0;
    do {
        pthread_create(&tid_w[i], NULL, buffer_write_handler, "sensor_data");
        i++;
    } while (i < NUM_WRITE_THREADS);
    do {
        pthread_create(&tid_r[j], NULL, buffer_read_handler, "sensor_data_out.csv");
        j++;
    } while (j < NUM_READ_THREADS);
    for (i = 0; i < NUM_WRITE_THREADS; i++)
        pthread_join(tid_w[i], NULL);
    for (j = 0; j < NUM_READ_THREADS; j++)
        pthread_join(tid_r[j], NULL);
    sbuffer_free(&buffer);
    return 0;
}

void *buffer_write_handler(void *arg) {
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    uint16_t sensor_id;
    double temperature;
    time_t timestamp;
    char *filename = (char*)arg;
    FILE *fp_sensor_data = fopen(filename, "rb");
    while(true){
        pthread_mutex_lock(&mutex);
        while(!feof(fp_sensor_data)) {
            // Read data of 1 sensor
            fread(&sensor_id, sizeof(sensor_id), 1, fp_sensor_data);
            fread(&temperature, sizeof(temperature), 1, fp_sensor_data);
            fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data);
            data->id = sensor_id;
            data->value = temperature;
            data->ts = timestamp;
            printf("%d %f %ld\n", sensor_id, temperature, timestamp);
            sbuffer_insert(buffer, data);
            usleep(10000); //wait 10ms after inserting a measurement
        }
        // end-of-stream marker (no more measurements)
        sensor_data_t eos_marker;
        eos_marker.id = 0;
        sbuffer_insert(buffer, &eos_marker);
        pthread_mutex_lock(&mutex);
    }
    fclose(fp_sensor_data);
    pthread_exit(0);
}

void *buffer_read_handler(void *arg) {
    sbuffer_node_t *node;
    sensor_data_t *data = malloc(sizeof(sensor_data_t*));
    char *filename = (char*)arg;
    FILE *fp_sensor_data_csv = fopen(filename, "a");
    bool end_of_stream = false;
    while (true) {
        pthread_mutex_lock(&mutex);
        while (!end_of_stream) {
            if (buffer->head->data.id != 0) {
                end_of_stream = true;
                sbuffer_remove(buffer, &(buffer->head->data));
            } else {
                node = buffer->head;
                *data = node->data;
                fprintf(fp_sensor_data_csv, "%d,%f,%ld\n", data->id, data->value, data->ts);
                sbuffer_remove(buffer, data);
                usleep(25000); //wait 25ms after writing a measurement
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    fclose(fp_sensor_data_csv);
    pthread_exit(0);
}