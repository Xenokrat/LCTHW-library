#include "minunit.h"
#include <assert.h>
#include <string.h>
#include "../src/lcthw/list.h"

static List *list = NULL;
char *test1 = "test1";
char *test2 = "test2";
char *test3 = "test3";

char *test_create()
{
    list = List_create();
    mu_assert(list != NULL, "Failed to create list");

    return NULL;
}

char *test_destroy()
{
    List_clear_destroy(list);
    
    return NULL;
}

char *test_push_pop()
{
    List_push(list, test1);
    mu_assert(List_last(list) == test1, "Wrong last value.");

    List_push(list, test2);
    mu_assert(List_last(list) == test2, "Wrong last value");

    List_push(list, test3);
    mu_assert(List_last(list) == test3, "Wrong last value");
    mu_assert(List_count(list) == 3, "Wrong count on push");

    char *val = List_pop(list);
    mu_assert(val == test3, "Wrong value on pop");

    val = List_pop(list);
    mu_assert(val == test2, "Wrong value on pop");

    val = List_pop(list);
    mu_assert(val == test1, "Wrong value on pop");
    mu_assert(List_count(list) == 0, "Wrong count after pop");

    return NULL;
}

char *test_unshift()
{
    List_unshift(list, test1);
    mu_assert(List_first(list) == test1, "Wrong first value");

    List_unshift(list, test2);
    mu_assert(List_first(list) == test2, "Wrong first value");

    List_unshift(list, test3);
    mu_assert(List_first(list) == test3, "Wrong fist value");
    mu_assert(List_count(list) == 3, "Wrong count on unshift");

    return NULL;
}

char *test_remove()
{
    // we only need to test the middle remove case since push/shfit
    // already tests the other case

    char *val = List_remove(list, list->first->next);
    mu_assert(val == test2, "Wrong removed element");
    mu_assert(List_count(list) == 2, "Wrong count after remove");
    mu_assert(List_first(list) == test3, "Wrong first after remomve");
    mu_assert(List_last(list) == test1, "Wrong last after remove");

    return NULL;
}

char *test_shift()
{
    mu_assert(List_count(list) != 0, "Wrong count before shift");

    char *val = List_shift(list);
    mu_assert(val == test3, "Wrong value on shift");

    val = List_shift(list);
    mu_assert(val == test1, "Wrong value on shift");
    mu_assert(List_count(list) == 0, "Wrong count after shift");
    
    return NULL;
}

char *test_find()
{
    List_push(list, test1);
    List_push(list, test2);
    List_push(list, test3);

    mu_assert(list->first->next == List_find(list, "test2", str_node_eq),
              "Wrong node found by List_find");

    // Cleanup
    List_pop(list);
    List_pop(list);
    List_pop(list);

    return NULL;
}

char *test_copy()
{
    List *list_cpy = List_copy(list);
    ListNode *cur_cpy = list_cpy->first;

    LIST_FOREACH(list, first, next, cur) {
        mu_assert(cur->value == cur_cpy->value, "Wrong during copy process");
        cur_cpy = cur_cpy->next;
    }

    List_destroy(list_cpy);
    return NULL;
}

char *test_join()
{
    List *list1 = List_create();
    List *list2 = List_create();

    char *test1 = "test1";
    char *test2 = "test2";
    char *test3 = "test3";
    List_push(list1, test1);
    List_push(list1, test2);
    List_push(list1, test3);

    char *test4 = "test4";
    char *test5 = "test5";
    char *test6 = "test6";
    List_push(list2, test4);
    List_push(list2, test5);
    List_push(list2, test6);

    /* finally test */
    List *list_join = List_join(list2, list);
    char *test_values[] = {
        "test4", "test5", "test6",
        "test1", "test2", "test3",
    };

    size_t i = 0;
    LIST_FOREACH(list_join, first, next, cur) {
        mu_assert(strcmp((char*)cur->value, (char*)test_values[i]) == 0, 
                  "Wrong value on test join");
        i++;
    }

    List_destroy(list1);
    List_destroy(list2);
    List_destroy(list_join);

    return NULL;
}

char *test_split()
{
    List *list_test = List_create();
    List_push(list_test, "test1");
    List_push(list_test, "test2");
    List_push(list_test, "test3");

    List *list_split = List_split(list_test, "test2", str_node_eq);


    mu_assert(
        strcmp(list_test->first->value, "test1") == 0, 
        "Wrong value on test split"
    );
    mu_assert(
        strcmp(list_test->first->next->value, "test2") == 0, 
        "Wrong value on test split"
    );
    mu_assert(list_test->first->next->next == NULL, "Wrong value on test split");

    mu_assert(
        strcmp(list_split->first->value, "test3") == 0,
        "Wrong value on test split"
    );

    List *list_split2 = List_split(list_test, "test10", str_node_eq);
    mu_assert(list_split2 == NULL, "Should return NULL here");
    mu_assert(
        strcmp(list_test->first->value, "test1") == 0, 
        "Should not modify list_test"
    );
    mu_assert(
        strcmp(list_test->first->next->value, "test2") == 0, 
        "Should not modify list_test"
    );

    List_destroy(list_test);
    List_destroy(list_split);
    List_destroy(list_split2);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_push_pop);
    mu_run_test(test_unshift);
    mu_run_test(test_remove);
    mu_run_test(test_shift);
    mu_run_test(test_find);
    mu_run_test(test_copy);
    mu_run_test(test_join);
    mu_run_test(test_split);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
