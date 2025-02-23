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

    char *values[] = {
        "test4", "test5", "test6",
        "test1", "test2", "test3"
    };

    List *list = List_join(list2, list1);

    mu_assert(check_consistency(list, values, (List_compare) strcmp) == 0,
              "Failed to List_join.");

    List_destroy(list);

    /* Second empty test */
    list1 = List_create();
    list2 = List_create();
    
    List_push(list2, test4);
    List_push(list2, test5);
    List_push(list2, test6);
    
    list = List_join(list2, list1);
    
    char *values2[] = {
        "test4", "test5", "test6",
    };
    
    mu_assert(check_consistency(list, values2, (List_compare) strcmp) == 0,
              "Failed to List_join when first empty.");
    
    List_destroy(list);

    return NULL;
}

char *test_split()
{
    List *first = NULL;
    List *second = NULL;
    
    first = List_create();
    /* test empty */
    second = List_split(first);
    mu_assert(second != NULL        &&
              second->first == NULL &&
              second->count == 0    &&
              first->first == NULL  &&
              first->count == 0,
              "Split failed on empty list");
    List_destroy(second);
    
    List_push(first, "test1");
    second = List_split(first);
    mu_assert(second != NULL        &&
              second->first == NULL &&
              second->count == 0    &&
              first->first != NULL  &&
              first->count == 1,
              "Split failed on one-element list");
    
    List_destroy(second);
    List_push(first, "test5");
    List_push(first, "test3");
    List_push(first, "test4");
    List_push(first, "test2");
    
    // char *values[] = { "test1", "test5", "test3", "test4", "test2" };
    char *values1[] = { "test1", "test5", "test3" };
    char *values2[] = { "test4", "test2" };
    second = List_split(first);
    mu_assert(check_consistency(first, values1, (List_compare) strcmp) == 0, 
              "Fail on splitting, first list incorrect!");
    mu_assert(check_consistency(second, values2, (List_compare) strcmp) == 0, 
              "Fail on splitting, second list incorrect!");
    mu_assert(first->count == 3, "Wrong count in `first` after splitting");
    mu_assert(second->count == 2, "Wrong count in `second` after splitting");
    List_destroy(second);
    List_destroy(first);
    
    first  = List_create();
    List_push(first, "test1");
    List_push(first, "test2");
    second = List_split(first);
    mu_assert(first->count == 1, "Test split2: wrong count on `first`");
    mu_assert(second->count == 1, "Test split2: wrong count on `second`");

    return NULL;
}

char *test_consistency()
{
    char *values[] = { "test1", "test2", "test3", "test4" };

    list = List_create();
    List_push(list, values[0]);
    List_push(list, values[1]);
    List_push(list, values[2]);
    List_push(list, values[3]);
    
    int rc = check_consistency(list, values, (List_compare) strcmp);

    mu_assert(rc == 0, "Fail to check consistency");
    List_destroy(list);

    return NULL;
}

char *test_shift_get()
{
    List *list = List_create();
    ListNode *node = NULL;

    /* Empty list test */
    node = List_shift_get(list);
    mu_assert(node == NULL, "Shifting node from empty list should give NULL");

    /* Single element list test */
    List_push(list, "test1");
    node = List_shift_get(list);
    
    mu_assert(strcmp((char*) node->value, "test1") == 0,
             "Shifting node should give element");
    mu_assert(list->count == 0 && list->first == NULL && list->last == NULL,
             "Single element List should be empty after shift get");
    free(node);

    /* More that 1-element list test */
    List_push(list, "test2");
    List_push(list, "test3");
    
    node = List_shift_get(list);
    mu_assert(strcmp((char*) node->value, "test2") == 0,
             "Shifting node should give `test2` element");
    mu_assert(list->count == 1 && 
             strcmp((char*) list->first->value, "test3") == 0,
             "List first element should be second now, and count correct");
    mu_assert(list->first->prev == NULL,
             "list->first-prev should point to NULL");

    List_destroy(list);
    return NULL;
}

char *test_join_sorted()
{
    List *list1 = List_create();
    List *list2 = List_create();
    
    List_push(list1, "test1");
    List_push(list1, "test3");
    List_push(list2, "test2");
    List_push(list2, "test4");

    List *list = List_join_sorted(list1, list2, (List_compare) strcmp);

    /* TODO */
    char *values[] = { "test1", "test2", "test3", "test4" };
    mu_assert(check_consistency(list, values, (List_compare) strcmp) == 0,
              "List_join_sorted failed");

    List_destroy(list);
    
    return NULL;
}

char *test_insert_sorted()
{
    List *list = List_create();

    /* insert into empty list */
    List_insert_sorted(list, "test2", (List_compare) strcmp);
    mu_assert(list->first == list->last, "List_insert_sorted failed");
    mu_assert(list->count == 1, "List_insert_sorted failed");
    mu_assert(strcmp((char*)list->first->value, "test2") == 0,
             "List_insert_sorted failed");
    
    /* insert more */
    List_insert_sorted(list, "test4", (List_compare) strcmp);
    mu_assert(list->count == 2, "List_insert_sorted failed (test4)");

    List_insert_sorted(list, "test1", (List_compare) strcmp);
    mu_assert(list->count == 3, "List_insert_sorted failed (test1)");
    
    List_insert_sorted(list, "test3", (List_compare) strcmp);
    mu_assert(list->count == 4, "List_insert_sorted failed (test3)");

    char *values[] = { "test1", "test2", "test3", "test4" };
    mu_assert(check_consistency(list, values, (List_compare) strcmp) == 0,
              "List_insert_sorted check_consistency failed");

    List_destroy(list);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_consistency);
    mu_run_test(test_create);
    mu_run_test(test_push_pop);
    mu_run_test(test_unshift);
    mu_run_test(test_remove);
    mu_run_test(test_shift);
    mu_run_test(test_find);
    mu_run_test(test_copy);
    mu_run_test(test_join);
    mu_run_test(test_split);
    mu_run_test(test_shift_get);
    mu_run_test(test_join_sorted);
    mu_run_test(test_insert_sorted);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
