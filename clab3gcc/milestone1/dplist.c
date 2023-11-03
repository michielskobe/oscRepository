

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
    /** Deletes all elements in the list
     * - Every list node of the list needs to be deleted. (free memory)
     * - The list itself also needs to be deleted. (free all memory)
     * - '*list' must be set to NULL.
     * \param list a double pointer to the list
     * \param free_element if true call element_free() on the element of the list node to remove
     */
    //TODO: add your code here
    dplist_node_t *current_node;

    if (*list != NULL) {
        while ((*list)->head) {
            current_node = (*list)->head;
            (*list)->head = (*list)->head->next;
            if (free_element && current_node->element != NULL) {
                // Set current_node->element to NULL before freeing it
                (*list)->element_free(&(current_node->element));
            }
            free(current_node);
        }
    }
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {
    /** Inserts a new list node containing an 'element' in the list at position 'index'
     * - the first list node has index 0.
     * - If 'index' is 0 or negative, the list node is inserted at the start of 'list'.
     * - If 'index' is bigger than the number of elements in the list, the list node is inserted at the end of the list.
     * - If 'list' is is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param element a pointer to the data that needs to be inserted
     * \param index the position at which the element should be inserted in the list
     * \param insert_copy if true use element_copy() to make a copy of 'element' and use the copy in the new list node, otherwise the given element pointer is added to the list
     * \return a pointer to the list or NULL
     */
    //TODO: add your code here
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;
    list_node = malloc(sizeof(dplist_node_t));
    if (insert_copy) {
        list_node->element = list->element_copy(element);
    }
    else {
        list_node->element = element;
    }
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

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    /** Removes the list node at index 'index' from the list.
     * - The list node itself should always be freed.
     * - If 'index' is 0 or negative, the first list node is removed.
     * - If 'index' is bigger than the number of elements in the list, the last list node is removed.
     * - If the list is empty, return the unmodified list.
     * - If 'list' is is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position at which the node should be removed from the list
     * \param free_element if true, call element_free() on the element of the list node to remove
     * \return a pointer to the list or NULL
     */
    //TODO: add your code here
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
    if (free_element)
    {
        list->element_free(&node_to_remove->element);
    }
    free(node_to_remove);
    return list;

}

int dpl_size(dplist_t *list) {
    /** Returns the number of elements in the list.
     * - If 'list' is is NULL, -1 is returned.
     * \param list a pointer to the list
     * \return the size of the list
     */
    //TODO: add your code here
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
    /** Returns the list element contained in the list node with index 'index' in the list.
     * - return is not returning a copy of the element with index 'index', i.e. 'element_copy()' is not used.
     * - If 'index' is 0 or negative, the element of the first list node is returned.
     * - If 'index' is bigger than the number of elements in the list, the element of the last list node is returned.
     * - If the list is empty, NULL is returned.
     * - If 'list' is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position of the node for which the element is returned
     * \return a pointer to the element at the given index or NULL
     */
    //TODO: add your code here
    void *element_at_index;
    if (list == NULL || list->head == NULL){
        element_at_index = 0;
    }
    else {
        element_at_index = dpl_get_reference_at_index(list, index)->element;
    }
    return element_at_index;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    /** Returns an index to the first list node in the list containing 'element'.
     * - the first list node has index 0.
     * - Use 'element_compare()' to search 'element' in the list, a match is found when 'element_compare()' returns 0.
     * - If 'element' is not found in the list, -1 is returned.
     * - If 'list' is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param element the element to look for
     * \return the index of the element that matches 'element'
     */
    //TODO: add your code here
    int index = -1;
    if (list == NULL) {
        return index;
    }
    else {
        int count = 0;
        dplist_node_t *current_node = list->head;
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

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    /** Returns a reference to the list node with index 'index' in the list.
     * - If 'index' is 0 or negative, a reference to the first list node is returned.
     * - If 'index' is bigger than the number of list nodes in the list, a reference to the last list node is returned.
     * - If the list is empty, NULL is returned.
     * - If 'list' is is NULL, NULL is returned.
     * \param list a pointer to the list
     * \param index the position of the node for which the reference is returned
     * \return a pointer to the list node at the given index or NULL
     */
    //TODO: add your code here
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

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    /** Returns the element contained in the list node with reference 'reference' in the list.
     * - If the list is empty, NULL is returned.
     * - If 'list' is is NULL, NULL is returned.
     * - If 'reference' is NULL, NULL is returned.
     * - If 'reference' is not an existing reference in the list, NULL is returned.
     * \param list a pointer to the list
     * \param reference a pointer to a certain node in the list
     * \return the element contained in the list node or NULL
     */
    //TODO: add your code here
    return NULL;

}


