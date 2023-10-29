#define _GNU_SOURCE
/**
 * \author Bert Lagaisse and Kobe Michiels
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *numbers = NULL;
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect case 1 for dpl_get_element_at_index()");
    ck_assert_msg(dpl_get_index_of_element(numbers, 'a') == -1, "Incorrect implementation for empty list in dpl_get_index_of_element()");
    numbers = dpl_create();

    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect case 2 for dpl_get_element_at_index()");
    ck_assert_msg(dpl_get_index_of_element(numbers, 'a') == -1, "Incorrect implementation for empty list in dpl_get_index_of_element()");


    dpl_insert_at_index(numbers, 'a', 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");

    dpl_insert_at_index(numbers, '\e', -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");

    dpl_insert_at_index(numbers, 'b', 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");

    ck_assert_msg(dpl_get_element_at_index(numbers, 1) == 'a', "Incorrect case 4 for dpl_get_element_at_index()");
    ck_assert_msg(dpl_get_element_at_index(numbers, 2) == 'b', "Incorrect case 3 for dpl_get_element_at_index()");
    ck_assert_msg(dpl_get_element_at_index(numbers, 20) == 'b', "Incorrect case 3 for dpl_get_element_at_index()");

    dpl_insert_at_index(numbers, 'c', 3);
    ck_assert_msg(dpl_get_element_at_index(numbers, 3) == 'c', "Incorrect case 3 for dpl_get_element_at_index()");
    dpl_remove_at_index(numbers, 3);
    ck_assert_msg(dpl_get_element_at_index(numbers, 3) == 'b', "Incorrect case 3 for dpl_get_element_at_index() or dpl_remove_at_index()");

    ck_assert_msg(dpl_get_index_of_element(numbers, '\e') == 0, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, 'a') == 1, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, 'b') == 2, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, 'c') == -1, "Incorrect implementation for element not found in dpl_get_index_of_element()");

    dpl_free(&numbers);



    return 0;
}