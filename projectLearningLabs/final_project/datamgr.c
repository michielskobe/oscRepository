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

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them.
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
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
    while(!feof(fp_sensor_data)) {
        // Read data of 1 sensor
        fread(&data_sensor_id, sizeof(data_sensor_id), 1, fp_sensor_data);
        fread(&temperature, sizeof(temperature), 1, fp_sensor_data);
        fread(&timestamp, sizeof(timestamp), 1, fp_sensor_data);

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

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    uint16_t room_id;
    dplist_node_t *sensor_node = datamgr_get_sensor_with_id(sensor_id);
    if (sensor_node != NULL) {
        my_element_t *sensor_element = sensor_node->element;
        room_id = sensor_element->room_id;
    }
    ERROR_HANDLER(sensor_node == NULL, "Invalid sensor_id");
    return room_id;
}

/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    double running_avg;
    dplist_node_t *sensor_node = datamgr_get_sensor_with_id(sensor_id);
    if (sensor_node != NULL) {
        my_element_t *sensor_element = sensor_node->element;
        running_avg = sensor_element->running_avg;
    }
    ERROR_HANDLER(sensor_node == NULL, "Invalid sensor_id");
    return running_avg;
}

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    time_t timestamp;
    dplist_node_t *sensor_node = datamgr_get_sensor_with_id(sensor_id);
    if (sensor_node != NULL) {
        my_element_t *sensor_element = sensor_node->element;
        timestamp = sensor_element->last_modified;
    }
    ERROR_HANDLER(sensor_node == NULL, "Invalid sensor_id");
    return timestamp;
}

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors(){
    return dpl_size(sensor_list);
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