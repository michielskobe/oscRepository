/**
 * \author Kobe Michiels
 */

#include "sbuffer.h"

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

pthread_mutex_t store_mutex;
pthread_mutex_t fetch_mutex;
pthread_cond_t store_cond;
pthread_cond_t fetch_cond;
int condition = 0;

int sbuffer_init(sbuffer_t **buffer) {
    pthread_mutex_init(&store_mutex, NULL);
    pthread_cond_init(&store_cond, NULL);

    pthread_mutex_init(&fetch_mutex, NULL);
    pthread_cond_init(&fetch_cond, NULL);

    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    pthread_mutex_destroy(&store_mutex);
    pthread_cond_destroy(&store_cond);
    pthread_mutex_destroy(&fetch_mutex);
    pthread_cond_destroy(&fetch_cond);
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) {
        return SBUFFER_FAILURE;
    }
    pthread_mutex_lock(&fetch_mutex);
    while(condition == 0){
        pthread_cond_wait(&fetch_cond, &fetch_mutex);
    }
    condition = 0;
    pthread_mutex_lock(&store_mutex);
    while (buffer->head == NULL) {
        pthread_cond_wait(&store_cond, &store_mutex);
    }
    *data = buffer->head->data;
    if (data->id == 0) {
        pthread_cond_signal(&fetch_cond);
        pthread_mutex_unlock(&fetch_mutex);
        pthread_mutex_unlock(&store_mutex);
        return SBUFFER_NO_DATA;
    }
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    pthread_cond_signal(&fetch_cond);
    pthread_mutex_unlock(&fetch_mutex);
    pthread_mutex_unlock(&store_mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&store_mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) {
        pthread_mutex_unlock(&store_mutex);
        return SBUFFER_FAILURE;
    }
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) {
        pthread_mutex_unlock(&store_mutex);
        return SBUFFER_FAILURE;
    }
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    if (data->id == 0){
        pthread_cond_broadcast(&store_cond);
        pthread_mutex_unlock(&store_mutex);
        return SBUFFER_NO_DATA;
    }
    pthread_cond_signal(&store_cond);
    pthread_mutex_unlock(&store_mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data) {
    if (buffer == NULL) {
        return SBUFFER_FAILURE;
    }
    pthread_mutex_lock(&fetch_mutex);
    while(condition == 1){
        pthread_cond_wait(&fetch_cond, &fetch_mutex);
    }
    condition = 1;
    pthread_mutex_lock(&store_mutex);
    while (buffer->head == NULL) {
        pthread_cond_wait(&store_cond, &store_mutex);
    }

    *data = buffer->head->data;
    if (data->id == 0) {
        pthread_cond_signal(&fetch_cond);
        pthread_mutex_unlock(&fetch_mutex);
        pthread_mutex_unlock(&store_mutex);
        return SBUFFER_NO_DATA;
    }
    pthread_cond_signal(&fetch_cond);
    pthread_mutex_unlock(&fetch_mutex);
    pthread_mutex_unlock(&store_mutex);
    return SBUFFER_SUCCESS;
}