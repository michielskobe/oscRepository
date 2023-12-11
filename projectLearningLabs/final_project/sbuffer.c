/**
 * \author Kobe Michiels
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t condvar;

int sbuffer_init(sbuffer_t **buffer) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Error: Initialization of mutex failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&condvar, NULL) != 0) {
        fprintf(stderr, "Error: Initialization of condition variable failed.\n");
        exit(EXIT_FAILURE);
    }
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
    if (pthread_mutex_destroy(&mutex) != 0) {
        fprintf(stderr, "Error: Destruction of mutex failed.\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_destroy(&condvar) != 0) {
        fprintf(stderr, "Error: Destruction of condition variable failed.\n");
        exit(EXIT_FAILURE);
    }
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_FAILURE;
    }
    while (buffer->head == NULL) {
        pthread_cond_wait(&condvar, &mutex);
    }
    *data = buffer->head->data;
    if (data->id == 0) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_SUCCESS;
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
    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_FAILURE;
    }
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) {
        pthread_mutex_unlock(&mutex);
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
        pthread_cond_broadcast(&condvar);
        pthread_mutex_unlock(&mutex);
        return SBUFFER_SUCCESS;
    }
    pthread_cond_signal(&condvar);
    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}