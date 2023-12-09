/**
 * \author Kobe Michiels
 */

#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"
#include "config.h"

typedef struct{
    uint16_t sensor_id;
    uint16_t room_id;
    double temperatures[RUN_AVG_LENGTH];
    int temperature_count;
    double running_avg;
    time_t last_modified;
} my_element_t;

struct dplist {
    dplist_node_t *head;
    void *(*element_copy)(void *src_element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
};

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

dplist_t *sensor_list;
sbuffer_t *buffer;

void *datamgr_parse_sensor_files(void *arg) {
    data_param_t *data_param = arg;
    buffer = data_param->buffer;
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    sensor_list = dpl_create(element_copy, element_free, element_compare);
    uint16_t sensor_id, room_id, data_sensor_id;
    double temperature;
    time_t timestamp;

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
    //insert sensor data at corresponding sensor node
    while(1) {
        // Read data of 1 sensor
        sbuffer_remove(buffer, data);
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
            printf("Sensor ID: %hd | Room ID: %hd | Running average: %.2f°C | Last modification: %ld\n", sensor->sensor_id, sensor->room_id, sensor->running_avg, sensor->last_modified);
            if (sensor->running_avg < SET_MIN_TEMP) {
                fprintf( stderr, "Too cold in room %hd\n",sensor->room_id);
            }
            else if (sensor->running_avg > SET_MAX_TEMP) {
                fprintf(stderr, "Too hot in room %hd\n",sensor->room_id);
            }
        } else {
            fprintf( stderr, "Data from unknown sensor %hd is ignored.\n",data_sensor_id);
        }
    }
}

/**
 * This method should be called to clean up the datamgr, and to free all used memory.
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free(){
    dpl_free(&sensor_list, true);
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