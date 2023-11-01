/**
 * \author Jeroen Van Aken, Bert Lagaisse, Ludo Bruynseels  and Kobe Michiels
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"



/*
 * The real definition of struct list / struct node
 */
struct dplist_node {
    dplist_node_t *prev, *next;
    element_t element;
};

struct dplist {
    dplist_node_t *head;
    // more fields will be added later
};

dplist_t *dpl_create() {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
  return list;
}

void dpl_free(dplist_t **list) {
    //Do extensive testing with valgrind.
    dplist_node_t *current_node;
    while(((*list)->head)) {
        current_node = (*list)->head;
        (*list)->head = (*list)->head->next;
        free(current_node);
    }
    free(*list);
    *list = NULL;
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/


dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));

    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index) {
    if (list == NULL) {
        return NULL;
    }
    // pointer drawing breakpoint
    dplist_node_t *node_to_remove = dpl_get_reference_at_index(list, index);
    if (list->head == NULL) { // covers case 1
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        dplist_node_t *new_next = node_to_remove->next;
        list->head = new_next;
        node_to_remove->next->prev = NULL;
        // pointer drawing breakpoint
    } else {
        // pointer drawing breakpoint
        if (index < dpl_size(list) - 1) { // covers case 4
            dplist_node_t *new_next = node_to_remove->next;
            dplist_node_t *new_prev = node_to_remove->prev;
            node_to_remove->next->prev = new_prev;
            node_to_remove->prev->next = new_next;
            // pointer drawing breakpoint
        } else { // covers case 3
            node_to_remove->prev->next = NULL;
            // pointer drawing breakpoint
        }
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
        dplist_node_t* current_node = list->head;
        while (current_node != NULL) {
            size++;
            current_node = current_node->next;
        }
        return size;
    }

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count = 0 ;
    dplist_node_t *dummy = NULL;
    if (list == NULL || list->head == NULL) {
        dummy = NULL;
    }
    else if (index <= 0) {
        dummy = list->head;
    }
    else if (index >= dpl_size(list)) {
        dplist_node_t* current_node = list->head;
        while (current_node->next != NULL) {
            current_node = current_node->next;
        }
        dummy = current_node;
    }
    else {
        dplist_node_t* current_node = list->head;
        while (count < index) {
            current_node = current_node->next;
            count++;
        }
        dummy = current_node;
    }
    return dummy;
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {
    element_t element_at_index;
    if (list == NULL || list->head == NULL){
        element_at_index = 0;
    }
    else {
        element_at_index = dpl_get_reference_at_index(list, index)->element;
    }
    return element_at_index;
}

int dpl_get_index_of_element(dplist_t *list, element_t element) {
    int index = -1;
    if (list == NULL) {
        return index;
    }
    else {
        int count = 0;
        dplist_node_t* current_node = list->head;
        while (current_node != NULL) {
            if (current_node->element == element) {
                index = count;
            }
            current_node = current_node->next;
            count++;
        }
        return index;
    }
}



