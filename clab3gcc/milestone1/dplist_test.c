#define _GNU_SOURCE

#include "dplist.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name); //asprintf requires _GNU_SOURCE
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}

void yourtest1()
    {
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Create variables to use in tests
        my_element_t *content1 = (my_element_t *)malloc(sizeof(my_element_t));
        content1->id = 5;
        content1->name = "foo";

        my_element_t *content2 = (my_element_t *)malloc(sizeof(my_element_t));
        content2->id = 3;
        content2->name = "boo";

        my_element_t *content3 = (my_element_t *)malloc(sizeof(my_element_t));
        content3->id = 2;
        content3->name = "hoo";

        int *content4 = malloc(sizeof(int));
        *content4 = 5;

        char *content5 = malloc(sizeof(char ));
        *content5 = 't';

        int *content6 = malloc(sizeof(int));
        *content6 = 65;

        // Test functions on list = NULL
        ck_assert_msg(dpl_size(list) == -1, "Failure: expected result to be -1.");
        ck_assert_msg(dpl_insert_at_index(list, content1, 0, true) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_insert_at_index(list, content1, 0, false) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_remove_at_index(list, 0, true) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_remove_at_index(list, 0, false) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_get_reference_at_index(list, 0) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_get_element_at_index(list, 0) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_get_index_of_element(list, content1) == -1, "Failure: expected result to be -1.");
        ck_assert_msg(dpl_get_element_at_reference(list, 0) == NULL, "Failure: expected result to be NULL.");

        list = dpl_create(element_copy, element_free, element_compare);

        // Test functions on empty list
        ck_assert_msg(dpl_size(list) == 0, "Failure: expected result to be 0.");
        ck_assert_msg(dpl_remove_at_index(list, 0, true) == list, "Failure: expected list to be unmodified.");
        ck_assert_msg(dpl_remove_at_index(list, 0, false) == list, "Failure: expected list to be unmodified.");
        ck_assert_msg(dpl_get_reference_at_index(list, 1) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_get_element_at_index(list, 1) == NULL, "Failure: expected result to be NULL.");
        ck_assert_msg(dpl_get_index_of_element(list, content1) == -1, "Failure: expected result to be -1.");
        ck_assert_msg(dpl_get_element_at_reference(list, 0) == NULL, "Failure: expected result to be NULL.");

        // Test dpl_remove_at_index() with only one element (special list->head->next = NULL edge-case), don't use callback
        dpl_insert_at_index(list, content1, 0, false);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, 1, false);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");
        dpl_insert_at_index(list, content1, 0, false);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, -1, false);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");
        dpl_insert_at_index(list, content1, 0, false);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, 10, false);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");

        // Test dpl_insert_at_index(), don't use callback
        dpl_insert_at_index(list, content1, 0, false);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_insert_at_index(list, content2, -1, false);
        ck_assert_msg(dpl_size(list) == 2, "Failure: Numbers must contain 2 elements.");
        dpl_insert_at_index(list, content3, 99, false);
        ck_assert_msg(dpl_size(list) == 3, "Failure: Numbers must contain 3 elements.");
        dpl_insert_at_index(list, content4, 3, false);
        ck_assert_msg(dpl_size(list) == 4, "Failure: Numbers must contain 4 elements.");
        dpl_insert_at_index(list, content5, 2, false);
        ck_assert_msg(dpl_size(list) == 5, "Failure: Numbers must contain 5 elements.");
        dpl_insert_at_index(list, content6, 2, false);
        ck_assert_msg(dpl_size(list) == 6, "Failure: Numbers must contain 6 elements.");

        // Test dpl_remove_at_index(), don't use callback
        dpl_remove_at_index(list, 1, false);
        ck_assert_msg(dpl_size(list) == 5, "Failure: Numbers must contain 5 element.");
        dpl_remove_at_index(list, 100, false);
        ck_assert_msg(dpl_size(list) == 4, "Failure: Numbers must contain 4 elements.");
        dpl_remove_at_index(list, -3, false);
        ck_assert_msg(dpl_size(list) == 3, "Failure: Numbers must contain 3 elements.");
        // Test dpl_remove_at_index() and dpl_get_element_at_index()
        ck_assert_msg(dpl_get_element_at_index(list, -1) == content6, "Failure: expected element at index 0 to be content6");
        ck_assert_msg(dpl_get_element_at_index(list, 0) == content6, "Failure: expected element at index 0 to be content6");
        ck_assert_msg(dpl_get_element_at_index(list, 1) == content5, "Failure: expected element at index 1 to be content5");
        ck_assert_msg(dpl_get_element_at_index(list, 2) == content3, "Failure: expected element at index 2 to be content3");
        ck_assert_msg(dpl_get_element_at_index(list, 99) == content3, "Failure: expected element at index 2 to be content3");

        // Test dpl_get_index_of_element()
        ck_assert_msg(dpl_get_index_of_element(list, content6) == 0, "Failure: expected result to be 0");
        ck_assert_msg(dpl_get_index_of_element(list, content3) == 2, "Failure: expected result to be 2");
        ck_assert_msg(dpl_get_index_of_element(list, content2) == -1, "Failure: expected result to be -1");

        // Test dpl_get_reference_at_index() and dpl_get_element_at_reference()
        dplist_node_t *node1 = dpl_get_reference_at_index(list, 2);
        void *el1 = dpl_get_element_at_reference(list, node1);
        ck_assert_msg(el1 == content3, "Failure: expected element at index 2 is content3");

        dplist_node_t *node2 = dpl_get_reference_at_index(list, -1);
        void *el2 = dpl_get_element_at_reference(list, node2);
        ck_assert_msg(el2 == content6, "Failure: expected element at index -1 is content6");

        dplist_node_t *node3 = dpl_get_reference_at_index(list, 0);
        void *el3 = dpl_get_element_at_reference(list, node3);
        ck_assert_msg(el3 == content6, "Failure: expected element at index 0 is content6");

        dplist_node_t *node4 = dpl_get_reference_at_index(list, 45);
        void *el4 = dpl_get_element_at_reference(list, node4);
        ck_assert_msg(el4 == content3, "Failure: expected element at index 45 is content3");

        void *non_existing_ref_el = dpl_get_element_at_reference(list, 0);
        ck_assert_msg(non_existing_ref_el == NULL, "Failure: expected result to be NULL");

        void *null_ref_el = dpl_get_element_at_reference(list, NULL);
        ck_assert_msg(null_ref_el == NULL, "Failure: expected result to be NULL");

        dpl_free(&list, false);

        list = dpl_create(element_copy, element_free, element_compare);

        // Test dpl_insert_at_index() and dpl_remove_at_index(), use callback with only one element (special list->head->next = NULL edge-case)
        dpl_insert_at_index(list, content1, 0, true);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, 1, true);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");
        dpl_insert_at_index(list, content1, 0, true);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, -1, true);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");
        dpl_insert_at_index(list, content1, 0, true);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_remove_at_index(list, 10, true);
        ck_assert_msg(dpl_size(list) == 0, "Failure: Numbers must contain 0 element.");

        // Test dpl_insert_at_index(), use callback
        dpl_insert_at_index(list, content1, 0, true);
        ck_assert_msg(dpl_size(list) == 1, "Failure: Numbers must contain 1 element.");
        dpl_insert_at_index(list, content2, -1, true);
        ck_assert_msg(dpl_size(list) == 2, "Failure: Numbers must contain 2 elements.");
        dpl_insert_at_index(list, content3, 99, true);
        ck_assert_msg(dpl_size(list) == 3, "Failure: Numbers must contain 3 elements.");
        dpl_insert_at_index(list, content4, 3, true);
        ck_assert_msg(dpl_size(list) == 4, "Failure: Numbers must contain 4 elements.");
        dpl_insert_at_index(list, content5, 2, true);
        ck_assert_msg(dpl_size(list) == 5, "Failure: Numbers must contain 5 elements.");
        dpl_insert_at_index(list, content6, 2, true);
        ck_assert_msg(dpl_size(list) == 6, "Failure: Numbers must contain 6 elements.");

        // Test dpl_remove_at_index(), use callback
        dpl_remove_at_index(list, 1, true);
        ck_assert_msg(dpl_size(list) == 5, "Failure: Numbers must contain 5 element.");
        dpl_remove_at_index(list, 100, true);
        ck_assert_msg(dpl_size(list) == 4, "Failure: Numbers must contain 4 elements.");
        dpl_remove_at_index(list, -3, true);
        ck_assert_msg(dpl_size(list) == 3, "Failure: Numbers must contain 3 elements.");
        // Test dpl_remove_at_index() and dpl_get_element_at_index()
        ck_assert_msg(element_compare(dpl_get_element_at_index(list, -1), content6) == 0, "Failure: expected element at index 0 to be content6");
        ck_assert_msg(element_compare(dpl_get_element_at_index(list, 0), content6) == 0, "Failure: expected element at index 0 to be content6");
        ck_assert_msg(element_compare(dpl_get_element_at_index(list, 1), content5) == 0, "Failure: expected element at index 1 to be content5");
        ck_assert_msg(element_compare(dpl_get_element_at_index(list, 2), content3) == 0, "Failure: expected element at index 2 to be content3");
        ck_assert_msg(element_compare(dpl_get_element_at_index(list, 99), content3) == 0, "Failure: expected element at index 2 to be content3");

        // Test dpl_get_index_of_element()
        ck_assert_msg(dpl_get_index_of_element(list, content6) == 0, "Failure: expected result to be 0");
        ck_assert_msg(dpl_get_index_of_element(list, content3) == 2, "Failure: expected result to be 2");
        ck_assert_msg(dpl_get_index_of_element(list, content2) == -1, "Failure: expected result to be -1");

        // Test dpl_get_reference_at_index() and dpl_get_element_at_reference()
        dplist_node_t *node5 = dpl_get_reference_at_index(list, 2);
        void *el5 = dpl_get_element_at_reference(list, node5);
        ck_assert_msg(element_compare(el5, content3) == 0, "Failure: expected element at index 2 is content3");

        dplist_node_t *node6 = dpl_get_reference_at_index(list, -1);
        void *el6 = dpl_get_element_at_reference(list, node6);
        ck_assert_msg(element_compare(el6, content6) == 0, "Failure: expected element at index -1 is content6");

        dplist_node_t *node7 = dpl_get_reference_at_index(list, 0);
        void *el7 = dpl_get_element_at_reference(list, node7);
        ck_assert_msg(element_compare(el7, content6) == 0, "Failure: expected element at index 0 is content6");

        dplist_node_t *node8 = dpl_get_reference_at_index(list, 45);
        void *el8 = dpl_get_element_at_reference(list, node8);
        ck_assert_msg(element_compare(el8, content3) == 0, "Failure: expected element at index 45 is content3");

        void *non_existing_ref_el2 = dpl_get_element_at_reference(list, 0);
        ck_assert_msg(non_existing_ref_el2 == NULL, "Failure: expected result to be NULL");

        void *null_ref_el2 = dpl_get_element_at_reference(list, NULL);
        ck_assert_msg(null_ref_el2 == NULL, "Failure: expected result to be NULL");

        dpl_free(&list, true);

        free(content1);
        free(content2);
        free(content3);
        free(content4);
        free(content5);
        free(content6);

        }




int main(void) {

    yourtest1();
    return 0;
}
