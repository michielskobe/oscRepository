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

        // TODO : your test scenarios
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

        list = dpl_create(element_copy, element_free, element_compare);

        dpl_insert_at_index(list, content1, 0, true);
        ck_assert_msg(dpl_size(list) == 1, "Numbers must contain 1 element.");

        dpl_insert_at_index(list, content2, -1, true);
        ck_assert_msg(dpl_size(list) == 2, "Numbers must contain 2 elements.");

        dpl_insert_at_index(list, content3, 99, true);
        ck_assert_msg(dpl_size(list) == 3, "Numbers must contain 3 elements.");

        dpl_remove_at_index(list, 2, true);
        ck_assert_msg(dpl_size(list) == 2, "Numbers must contain 2 elements.");

        dpl_insert_at_index(list, content4, 6, true);
        ck_assert_msg(dpl_size(list) == 3, "Numbers must contain 3 elements.");

        dpl_free(&list, true);

        list = dpl_create(element_copy, element_free, element_compare);
        dpl_insert_at_index(list, content1, 0, false);
        ck_assert_msg(dpl_size(list) == 1, "Numbers must contain 1 element.");

        dpl_insert_at_index(list, content2, -1, false);
        ck_assert_msg(dpl_size(list) == 2, "Numbers must contain 2 elements.");

        dpl_insert_at_index(list, content3, 99, false);
        ck_assert_msg(dpl_size(list) == 3, "Numbers must contain 3 elements.");

        dpl_remove_at_index(list, 2, false);
        ck_assert_msg(dpl_size(list) == 2, "Numbers must contain 2 elements.");

        dplist_node_t *node = (dplist_node_t *) dpl_get_reference_at_index(list, 1);
        my_element_t *el = (my_element_t *) dpl_get_element_at_reference(list, node);
        ck_assert_msg(el == content1, "Failure: expected element at 1 is content1");

        my_element_t *non_existing_el = (my_element_t *) dpl_get_element_at_reference(list, (dplist_node_t *) 0x5555555593a1);
        ck_assert_msg(non_existing_el == NULL, "Failure: expected result to be NULL");

        ck_assert_msg(dpl_get_index_of_element(list, content1) == 1, "Failure: expected result to be 1");
        ck_assert_msg(dpl_get_index_of_element(list, content3) == -1, "Failure: expected result to be -1");

        dpl_insert_at_index(list, content4, 6, false);
        ck_assert_msg(dpl_size(list) == 3, "Numbers must contain 3 elements.");

        dpl_free(&list, false);

        free(content1);
        free(content2);
        free(content3);

        }




int main(void) {

    yourtest1();
    return 0;
}
