

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"




/*
 * The real definition of struct list / struct node
 */

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


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    dplist_node_t *current_node;

    if (*list != NULL) {
        while ((*list)->head) {
            current_node = (*list)->head;
            (*list)->head = (*list)->head->next;
            if (free_element && current_node->element != NULL) {
                (*list)->element_free(&(current_node->element));
            }
            free(current_node);
        }
    }
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;
    list_node = malloc(sizeof(dplist_node_t));
    if (insert_copy) {
        list_node->element = list->element_copy(element);
    }
    else {
        list_node->element = element;
    }
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
        }
    }
    return list;

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if (list == NULL) {
        return NULL;
    }
    dplist_node_t *node_to_remove = dpl_get_reference_at_index(list, index);
    if (list->head == NULL) { // covers case 1
    } else if (index <= 0) { // covers case 2
        dplist_node_t *new_next = node_to_remove->next;
        list->head = new_next;
        node_to_remove->next->prev = NULL;
    } else {
        if (index < dpl_size(list) - 1) { // covers case 4
            dplist_node_t *new_next = node_to_remove->next;
            dplist_node_t *new_prev = node_to_remove->prev;
            node_to_remove->next->prev = new_prev;
            node_to_remove->prev->next = new_next;
        } else { // covers case 3
            node_to_remove->prev->next = NULL;
        }
    }
    if (free_element)
    {
        list->element_free(&node_to_remove->element);
    }
    free(node_to_remove);
    return list;

}

int dpl_size(dplist_t *list) {
    if (list == NULL) {
        return -1;
    }
    else {
        int size = 0;
        dplist_node_t *current_node = list->head;
        while (current_node != NULL) {
            size++;
            current_node = current_node->next;
        }
        return size;
    }
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL){
        return NULL;
    } else {
        return dpl_get_reference_at_index(list, index)->element;
    }
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    int index = -1;
    if (list == NULL) {
        return index;
    }
    else {
        int count = 0;
        dplist_node_t *current_node = list->head;
        while (current_node != NULL) {
            if (list->element_compare(current_node->element,element) == 0) {
                index = count;
            }
            current_node = current_node->next;
            count++;
        }
        return index;
    }
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count = 0 ;
    dplist_node_t *node_to_return = NULL;
    if (list == NULL || list->head == NULL) {
        node_to_return = NULL;
    }
    else if (index <= 0) {
        node_to_return = list->head;
    }
    else if (index >= dpl_size(list)) {
        dplist_node_t* current_node = list->head;
        while (current_node->next != NULL) {
            current_node = current_node->next;
        }
        node_to_return = current_node;
    }
    else {
        dplist_node_t* current_node = list->head;
        while (count < index) {
            current_node = current_node->next;
            count++;
        }
        node_to_return = current_node;
    }
    return node_to_return;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL || reference == NULL) {
        return NULL;
    }
    dplist_node_t *current_node = list->head;
    while (current_node != NULL) {
        if (current_node == reference) {
            return current_node->element;
        }
        current_node = current_node->next;
    }
    return NULL;
}


