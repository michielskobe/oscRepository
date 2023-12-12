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


int fd_datamgr;
char log_msg_datamgr[SIZE];
sbuffer_t *buffer_datamgr;
dplist_t *sensor_list;

void *data_manager(void *arg) {
    data_thread_arg_t *data_thread_arg = arg;
    buffer_datamgr = data_thread_arg->buffer;
    fd_datamgr = data_thread_arg->fd;

    // create data list for sensors
    sensor_list = dpl_create(element_copy, element_free, element_compare);

    // fill list with elements containing sensor and room id's
    uint16_t sensor_id, room_id;
    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    my_element_t *sensor = malloc(sizeof(my_element_t));
    while(!feof(fp_sensor_map)) {
        fscanf(fp_sensor_map, "%hd %hd\n", &room_id, &sensor_id);
        assert(sensor != NULL);
        sensor->sensor_id = sensor_id;
        sensor->room_id = room_id;
        sensor->temperature_count = 0;
        dpl_insert_at_index(sensor_list,sensor, dpl_size(sensor_list),true);
    }
    fclose(fp_sensor_map);

    // insert buffer data at corresponding node of sensor list
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    uint16_t buffer_data_id;
    double temperature;
    time_t timestamp;
    while(sbuffer_read(buffer_datamgr, data) != SBUFFER_NO_DATA) {
        buffer_data_id = data->id;
        temperature = data->value;
        timestamp = data->ts;
        dplist_node_t *sensor_node = get_sensor_with_id(buffer_data_id);
        if (sensor_node != NULL) {
            // calculate running average
            my_element_t *sensor_element = sensor_node->element;
            int index = (sensor_element->temperature_count)%RUN_AVG_LENGTH;
            if (index < RUN_AVG_LENGTH) {
                sensor_element->temperatures[index] = temperature;
                (sensor_element->temperature_count)++;
                if (sensor_element->temperature_count >= RUN_AVG_LENGTH) {
                    // Calculate running average:
                    double temperature_sum = 0;
                    for (int i = 0; i < RUN_AVG_LENGTH; i++) {
                        temperature_sum += sensor_element->temperatures[i];
                    }
                    sensor_element->running_avg = (temperature_sum/RUN_AVG_LENGTH);
                    sensor_element->last_modified = timestamp;
                }
            }
            else {
                ERROR_HANDLER((index >= RUN_AVG_LENGTH), "Index out of bounds");
            }
            // internal decision-making based on running average and temperature bounds
            if (sensor_element->running_avg < SET_MIN_TEMP && sensor_element->temperature_count >= RUN_AVG_LENGTH) {
                sprintf(log_msg_datamgr, "Sensor node %" PRIu16 " reports it’s too cold (avg temp = %f)", sensor_element->sensor_id, sensor_element->running_avg);
                write(fd_datamgr, log_msg_datamgr, SIZE);
            }
            else if (sensor_element->running_avg > SET_MAX_TEMP && sensor_element->temperature_count >= RUN_AVG_LENGTH) {
                sprintf(log_msg_datamgr, "Sensor node %" PRIu16 " reports it’s too hot (avg temp = %f)", sensor_element->sensor_id, sensor_element->running_avg);
                write(fd_datamgr, log_msg_datamgr, SIZE);
            }
        } else {
            sprintf(log_msg_datamgr, "Received sensor data with invalid sensor node ID %" PRIu16 " ", buffer_data_id);
            write(fd_datamgr, log_msg_datamgr, SIZE);
        }
    }
    // free allocated memory resources
    free(sensor);
    free(data);
    dpl_free(&sensor_list, true);
    pthread_exit(0);
}

dplist_node_t *get_sensor_with_id(sensor_id_t id) {
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
