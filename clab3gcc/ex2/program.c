#define _GNU_SOURCE
/**
 * \author Bert Lagaisse and Kobe Michiels
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include "dplist.h"
#include "string.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *numbers = NULL;
    ck_assert_msg(dpl_size(numbers) == -1, "Incorrect implementation for list = NULL of dpl_size-function");
    ck_assert_msg(dpl_insert_at_index(numbers, 0, 0) == NULL, "Incorrect implementation for list = NULL of dpl_insert_at_index()-function");
    ck_assert_msg(dpl_remove_at_index(numbers, 0) == NULL, "Incorrect implementation for list = NULL of dpl_remove_at_index()-function");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 0) == NULL, "Incorrect implementation for list = NULL of dpl_get_reference_at_index()-function");
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect implementation for list = NULL of dpl_get_element_at_index()-function");
    ck_assert_msg(dpl_get_index_of_element(numbers, "rat") == -1, "Incorrect implementation for list = NULL of dpl_get_index_of_element()");

    numbers = dpl_create();
    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements.");
    ck_assert_msg(dpl_remove_at_index(numbers, 1) == numbers, "Incorrect implementation for empty list case of dpl_remove_at_index()");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 1) == NULL, "Incorrect implementation for empty list case of dpl_get_reference_at_index()");
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect implementation for empty list case of dpl_get_element_at_index()");
    ck_assert_msg(dpl_get_index_of_element(numbers, "rat") == -1, "Incorrect implementation for empty list in dpl_get_index_of_element()");

    dpl_insert_at_index(numbers, "a", 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,0),"a"), "Incorrect implementation of dpl_insert_at_index()");
    dpl_insert_at_index(numbers, "\e", -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,0),"\e"), "Incorrect implementation of dpl_insert_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,1),"a"), "Incorrect implementation of dpl_insert_at_index()");
    dpl_insert_at_index(numbers, "b", 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,2),"b"), "Incorrect implementation of dpl_insert_at_index()");
    dpl_insert_at_index(numbers, "x", 2);
    ck_assert_msg(dpl_size(numbers) == 4, "Numbers must contain 4 elements.");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,2), "x"), "Incorrect implementation of dpl_insert_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,3),"b"), "Incorrect implementation of dpl_insert_at_index()");
    dpl_insert_at_index(numbers, "c", 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3),"c"), "Incorrect implementation of dpl_get_element_at_index()");
    dpl_remove_at_index(numbers, 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3),"b"), "Incorrect implementation of dpl_remove_at_index()");
    dpl_remove_at_index(numbers, 2);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2),"x"), "Incorrect implementation of dpl_remove_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2),"b"), "Incorrect implementation of dpl_remove_at_index()");
    dpl_remove_at_index(numbers, -1);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "\e"), "Incorrect implementation of dpl_remove_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0),"a"), "Incorrect implementation of dpl_remove_at_index()");
    dpl_insert_at_index(numbers, "\e", -1);
    dpl_insert_at_index(numbers, "c", 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3) , "c"), "Incorrect implementation of dpl_get_element_at_index()");
    dpl_remove_at_index(numbers, 10);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2) , "b"), "Incorrect implementation of dpl_remove_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3) , "b"), "Incorrect implementation of dpl_remove_at_index()");

    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, -1) , "\e"), "Incorrect implementation of dpl_get_element_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0) , "\e"), "Incorrect implementation of dpl_get_element_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 1) , "a"), "Incorrect implementation of dpl_get_element_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2) , "b"), "Incorrect implementation of dpl_get_element_at_index()");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 20) , "b"), "Incorrect implementation of dpl_get_element_at_index()");

    ck_assert_msg(dpl_get_index_of_element(numbers, "\e") == 0, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, "a") == 1, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, "b") == 2, "Incorrect implementation of dpl_get_index_of_element()");
    ck_assert_msg(dpl_get_index_of_element(numbers, "c") == -1, "Incorrect implementation of dpl_get_index_of_element()");

    ck_assert_msg(dpl_get_reference_at_index(numbers, 0) != NULL, "Incorrect implementation of dpl_get_reference_at_index()");
    ck_assert_msg(dpl_get_reference_at_index(numbers, -1) != NULL, "Incorrect implementation of dpl_get_reference_at_index()");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 1) != NULL, "Incorrect implementation of dpl_get_reference_at_index()");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 5) != NULL, "Incorrect implementation of dpl_get_reference_at_index()");


    dpl_free(&numbers);
    return 0;
}