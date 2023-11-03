#define _GNU_SOURCE
/**
 * \author Bert Lagaisse and Kobe Michiels
 *
 * main method that executes some test functions (without check framework)
 */

#include <stdio.h>
#include <stdlib.h>
#include "dplist.h"
#include "string.h"

void ck_assert_msg(bool result, char * msg){
    if(!result) printf("%s\n", msg);
}
int main(void)
{
    dplist_t *numbers = NULL;
    ck_assert_msg(dpl_size(numbers) == -1, "Incorrect implementation for list = NULL of dpl_size-function [Test1]");
    ck_assert_msg(dpl_insert_at_index(numbers, 0, 0) == NULL, "Incorrect implementation for list = NULL of dpl_insert_at_index()-function [Test2]");
    ck_assert_msg(dpl_remove_at_index(numbers, 0) == NULL, "Incorrect implementation for list = NULL of dpl_remove_at_index()-function [Test3]");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 0) == NULL, "Incorrect implementation for list = NULL of dpl_get_reference_at_index()-function [Test4]");
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect implementation for list = NULL of dpl_get_element_at_index()-function [Test5]");
    ck_assert_msg(dpl_get_index_of_element(numbers, "test") == -1, "Incorrect implementation for list = NULL of dpl_get_index_of_element() [Test6]");

    numbers = dpl_create();
    ck_assert_msg(numbers != NULL, "numbers = NULL, List not created [Test7]");
    ck_assert_msg(dpl_size(numbers) == 0, "Numbers may not contain elements. [Test8]");
    ck_assert_msg(dpl_remove_at_index(numbers, 1) == numbers, "Incorrect implementation for empty list case of dpl_remove_at_index() [Test9]");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 1) == NULL, "Incorrect implementation for empty list case of dpl_get_reference_at_index() [Test10]");
    ck_assert_msg(dpl_get_element_at_index(numbers, 0) == 0, "Incorrect implementation for empty list case of dpl_get_element_at_index() [Test11]");
    ck_assert_msg(dpl_get_index_of_element(numbers, "test") == -1, "Incorrect implementation for empty list in dpl_get_index_of_element() [Test12]");

    dpl_insert_at_index(numbers, "azerty", 0);
    ck_assert_msg(dpl_size(numbers) == 1, "Numbers must contain 1 element. [Test13]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,0),"azerty") == 0, "Incorrect implementation of dpl_insert_at_index() [Test14]");
    dpl_insert_at_index(numbers, "\e", -1);
    ck_assert_msg(dpl_size(numbers) == 2, "Numbers must contain 2 elements. [Test15]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,0),"\e") == 0, "Incorrect implementation of dpl_insert_at_index() [Test16]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,1),"azerty") == 0, "Incorrect implementation of dpl_insert_at_index() [Test17]");
    dpl_insert_at_index(numbers, "string", 100);
    ck_assert_msg(dpl_size(numbers) == 3, "Numbers must contain 3 elements. [Test18]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,2),"string") == 0, "Incorrect implementation of dpl_insert_at_index() [Test19]");
    dpl_insert_at_index(numbers, "x", 2);
    ck_assert_msg(dpl_size(numbers) == 4, "Numbers must contain 4 elements. [Test20]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,2), "x") == 0, "Incorrect implementation of dpl_insert_at_index() [Test21]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers,3),"string") == 0, "Incorrect implementation of dpl_insert_at_index() [Test22]");
    dpl_insert_at_index(numbers, "c3", 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3),"c3") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test23]");
    dpl_remove_at_index(numbers, 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3),"string") == 0, "Incorrect implementation of dpl_remove_at_index() [Test24]");
    dpl_remove_at_index(numbers, 2);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2),"x") != 0, "Incorrect implementation of dpl_remove_at_index() [Test25]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2),"string") == 0, "Incorrect implementation of dpl_remove_at_index() [Test26]");
    dpl_remove_at_index(numbers, -1);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0), "\e") != 0, "Incorrect implementation of dpl_remove_at_index() [Test27]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0),"azerty") == 0, "Incorrect implementation of dpl_remove_at_index() [Test28]");
    dpl_insert_at_index(numbers, "\e", -1);
    dpl_insert_at_index(numbers, "c3", 3);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3) , "c3") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test29]");
    dpl_remove_at_index(numbers, 10);
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2) , "string") == 0, "Incorrect implementation of dpl_remove_at_index() [Test30]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 3) , "string") == 0, "Incorrect implementation of dpl_remove_at_index() [Test31]");

    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, -1) , "\e") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test32]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 0) , "\e") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test33]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 1) , "azerty") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test34]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 2) , "string") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test35]");
    ck_assert_msg(strcmp(dpl_get_element_at_index(numbers, 20) , "string") == 0, "Incorrect implementation of dpl_get_element_at_index() [Test36]");

    ck_assert_msg(dpl_get_index_of_element(numbers, "\e") == 0, "Incorrect implementation of dpl_get_index_of_element() [Test37]");
    ck_assert_msg(dpl_get_index_of_element(numbers, "azerty") == 1, "Incorrect implementation of dpl_get_index_of_element() [Test38]");
    ck_assert_msg(dpl_get_index_of_element(numbers, "string") == 2, "Incorrect implementation of dpl_get_index_of_element() [Test39]");
    ck_assert_msg(dpl_get_index_of_element(numbers, "c3") == -1, "Incorrect implementation of dpl_get_index_of_element() [Test40]");

    ck_assert_msg(dpl_get_reference_at_index(numbers, 0) != NULL, "Incorrect implementation of dpl_get_reference_at_index() [Test41]");
    ck_assert_msg(dpl_get_reference_at_index(numbers, -1) != NULL, "Incorrect implementation of dpl_get_reference_at_index() [Test42]");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 1) != NULL, "Incorrect implementation of dpl_get_reference_at_index() [Test43]");
    ck_assert_msg(dpl_get_reference_at_index(numbers, 5) != NULL, "Incorrect implementation of dpl_get_reference_at_index() [Test44]");


    dpl_free(&numbers);
    return 0;
}