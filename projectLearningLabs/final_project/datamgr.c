/**
 * \author Kobe Michiels
 */

#include "datamgr.h"

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;
    void *(*element_copy)(void *src_element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
};

typedef struct{
    uint16_t sensor_id;
    uint16_t room_id;
    double temperatures[RUN_AVG_LENGTH];
    int temperature_count;
    double running_avg;
    time_t last_modified;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    assert(copy != NULL);
    copy->sensor_id = ((my_element_t*)element)->sensor_id;
    copy->room_id = ((my_element_t*)element)->room_id;
    for (int i = 0; i < RUN_AVG_LENGTH; i++){
        copy->temperatures[i] = ((my_element_t*)element)->temperatures[i];
    }
    copy->temperature_count = ((my_element_t*)element)->temperature_count;
    copy->running_avg = ((my_element_t*)element)->running_avg;
    copy->last_modified = ((my_element_t*)element)->last_modified;
    return (void *) copy;
}

void element_free(void ** element) {
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->sensor_id < ((my_element_t*)y)->sensor_id) ? -1 : (((my_element_t*)x)->sensor_id == ((my_element_t*)y)->sensor_id) ? 0 : 1);
}


int data_log_fd;
char data_log_msg[SIZE];
sbuffer_t *data_buffer;
dplist_t *sensor_list;

void *data_manager(void *arg) {
    data_param_t *data_param = arg;
    data_buffer = data_param->buffer;
    data_log_fd = data_param->write_end;

    sensor_list = dpl_create(element_copy, element_free, element_compare);
    uint16_t sensor_id, room_id, data_sensor_id;
    double temperature;
    time_t timestamp;

    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    //insert sensor and room id's into list
    while(!feof(fp_sensor_map)) {
        fscanf(fp_sensor_map, "%hd %hd\n", &room_id, &sensor_id);
        my_element_t *sensor = malloc(sizeof(my_element_t));
        assert(sensor != NULL);
        sensor->sensor_id = sensor_id;
        sensor->room_id = room_id;
        sensor->temperature_count = 0;
        dpl_insert_at_index(sensor_list,sensor, dpl_size(sensor_list),false);
    }
    fclose(fp_sensor_map);
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    //insert buffer data at corresponding sensor node
    while(1) {
        // Read data of 1 sensor
        sbuffer_remove(data_buffer, data);
        data_sensor_id = data->id;
        temperature = data->value;
        timestamp = data->ts;
        dplist_node_t *sensor_node = datamgr_get_sensor_with_id(data_sensor_id);
        if (sensor_node != NULL) {
            my_element_t *sensor = sensor_node->element;
            int index = (sensor->temperature_count)%RUN_AVG_LENGTH;
            if (index < RUN_AVG_LENGTH) {
                sensor->temperatures[index] = temperature;
                (sensor->temperature_count)++;
                if (sensor->temperature_count >= RUN_AVG_LENGTH) {
                    // Calculate running average:
                    double temperature_sum = 0;
                    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
                        temperature_sum += sensor->temperatures[i];
                    }
                    sensor->running_avg = (temperature_sum/RUN_AVG_LENGTH);
                    sensor->last_modified = timestamp;
                }
            }
            else {
                ERROR_HANDLER((index >= RUN_AVG_LENGTH), "Index out of bounds");
            }
            //printf("Sensor ID: %hd | Room ID: %hd | Running average: %.2f°C | Last modification: %ld\n", sensor->sensor_id, sensor->room_id, sensor->running_avg, sensor->last_modified);
            if (sensor->running_avg < SET_MIN_TEMP && sensor->temperature_count >= RUN_AVG_LENGTH) {
                //fprintf( stderr, "Too cold in room %hd\n",sensor->room_id);
                sprintf(data_log_msg, "Sensor node %" PRIu16 " reports it’s too cold (avg temp = %f)", sensor->sensor_id, sensor->running_avg);
                write(data_log_fd, data_log_msg, SIZE);
            }
            else if (sensor->running_avg > SET_MAX_TEMP && sensor->temperature_count >= RUN_AVG_LENGTH) {
                //fprintf(stderr, "Too hot in room %hd\n",sensor->room_id);
                sprintf(data_log_msg, "Sensor node %" PRIu16 " reports it’s too hot (avg temp = %f)", sensor->sensor_id, sensor->running_avg);
                write(data_log_fd, data_log_msg, SIZE);
            }
        } else {
            //fprintf( stderr, "Data from unknown sensor %hd is ignored.\n",data_sensor_id);
            sprintf(data_log_msg, "Received sensor data with invalid sensor node ID %" PRIu16 " ", data_sensor_id);
            write(data_log_fd, data_log_msg, SIZE);
        }
    }
    pthread_exit(0);
}

dplist_node_t *datamgr_get_sensor_with_id(sensor_id_t id) {
    dplist_node_t *current_node = sensor_list->head;
    while(current_node != NULL){
        uint16_t current_node_id = ((my_element_t*)current_node->element)->sensor_id;
        if (current_node_id == id) {
            return current_node;
        }
        current_node = current_node->next;
    }
    return NULL;
}
