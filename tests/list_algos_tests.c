#include <string.h>
#include <assert.h>
#include "../src/lcthw/list_algos.h"
#include "minunit.h"

char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS" };

#define NUM_VALUES 5

List *create_words()
{
    int i = 0;
    List *words = List_create();

    for (i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }

    return words;
}

char *test_swap_nodes()
{
    List *list = List_create();

    List_push(list, "test1");
    List_push(list, "test2");
    List_push(list, "test3");
    List_push(list, "test4");

    /* Setup nodes */
    ListNode *node1 = list->first;      /* test1 */
    ListNode *node2 = list->last->prev; /* test3 */
    
    /* Now:
     * test1 -> test2 -> test3 -> test4
     */
    swap_nodes(node1, node2);
    /* Should be:
     * test3 -> test2 -> test1 -> test4
     */

    char *tst[] = { "test3", "test2", "test1", "test4" };
    int tst_len = 4;
    int i = 0;
    ListNode *cur = NULL;
    
    /* Test List OK */
    mu_assert(list->first->prev == NULL,
              "Fail list->first->prev points NOT on NULL");
    mu_assert(list->last->next == NULL,
              "Fail list->last->next points NOT on NULL");

    /* Test forward */
    for (i = 0, cur = list->first; 
         cur != NULL && i < tst_len; 
         i++, cur = cur->next) 
    {
        mu_assert(strcmp((char*) cur->value, tst[i]) == 0,
                  "Fail to swap non-adjacent nodes correctly (forward)");
    }
    
    /* Also tests backwards */
    for (i = tst_len - 1, cur = list->last; 
         cur != NULL && i >= 0; 
         i--, cur = cur->prev) 
    {
        mu_assert(strcmp((char*) cur->value, tst[i]) == 0,
                  "Fail to swap non-adjacent nodes correctly (backward)");
    }
    
    /* Another round for ajustent nodes */
    swap_nodes(list->first, list->first->next);
    
    char *tst2[] = { "test2", "test3", "test1", "test4" };
    i = 0;
    cur = NULL;

    /* Test forward again */
    for (i = 0, cur = list->first; 
         cur != NULL && i < tst_len; 
         i++, cur = cur->next) 
    {
        mu_assert(strcmp((char*) cur->value, tst2[i]) == 0,
                  "Fail to swap adjacent nodes correctly (forward)");
    }
    
    /* Also tests backwards again */
    for (i = tst_len - 1, cur = list->last; 
         cur != NULL && i >= 0; 
         i--, cur = cur->prev) 
    {
        mu_assert(strcmp((char*) cur->value, tst2[i]) == 0,
                  "Fail to swap adjacent nodes correctly (backward)");
    }

    /* Cleanup if ok */
    List_pop(list);
    List_pop(list);
    List_pop(list);
    List_pop(list);
    List_destroy(list);

    return NULL;
}

char *test_bubble_sort()
{
    List *words = create_words();

    // should work on a list that needs sorting
    int rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words),
              "Words are not sorted after bubble sort.");

    // should work on an already sorted list
    rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort of already sorted failed.");
    mu_assert(is_sorted(words),
              "Words should be sort if already bubble sorted.");

    List_destroy(words); 

    // should work on empty list
    words = List_create();
    rc = List_bubble_sort(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed on empty list.");
    mu_assert(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *test_bubble_sort_opt()
{
    List *words = create_words();

    // should work on a list that needs sorting
    int rc = List_bubble_sort_opt(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words),
              "Words are not sorted after bubble sort opt.");

    // should work on an already sorted list
    rc = List_bubble_sort_opt(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort opt of already sorted failed.");
    mu_assert(is_sorted(words),
              "Words should be sorted if already bubble sorted.");

    List_destroy(words); 

    // should work on empty list
    words = List_create();
    rc = List_bubble_sort_opt(words, (List_compare) strcmp);
    mu_assert(rc == 0, "Bubble sort opt failed on empty list.");
    mu_assert(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *merge_sort_base_test()
{
    List *list = List_create();
    List_push(list, "test1");
    
    list = List_merge_sort(list, (List_compare) strcmp);
    mu_assert(strcmp((char*) list->first->value, "test1") == 0,
              "Merge sort base case failed")

    List_pop(list);
    List_destroy(list);
    return NULL;
}

char *merge_sort_test()
{
    List *words = create_words();

    // should work on a list that needs sorting
    List *res = List_merge_sort(words, (List_compare) strcmp);
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

    List *res2 = List_merge_sort(res, (List_compare) strcmp);
    mu_assert(is_sorted(res2),
              "Should still be sorted after merge sort.");
    // List_destroy(res2);
    // List_destroy(res);

    List_destroy(words);
    return NULL;
}

char *merge_test()
{
    List* left  = List_create();
    List* right = List_create();
    
    List_push(left, "test1");
    List_push(left, "test3");
    List_push(right, "test2");
    List_push(right, "test4");

    List *merged = List_merge(left, right, (List_compare) strcmp);
    char *res[] = { "test1", "test2", "test3", "test4" };

    mu_assert(check_consistency(merged, res, (List_compare) strcmp) == 0,
             "Failed to merge_sort");

    return NULL;
}

char *listnode_merge_test()
{
    ListNode* res = NULL;
    List* list1 = List_create();
    List* list2 = List_create();

    /* both NULL returns NULL */
    res = ListNode_merge(list1->first, list2->first, (List_compare) strcmp);
    mu_assert(res == NULL, "Result of merging NULLs should be NULL");
    
    /* one NULL returns other */
    List_push(list1, "test1");
    List_push(list1, "test3");
    List_push(list1, "test5");
    
    res = ListNode_merge(list1->first, list2->first, (List_compare) strcmp);
    mu_assert(
        strcmp((char*) res->value,             "test1") == 0 && 
        strcmp((char*) res->next->value,       "test3") == 0 && 
        strcmp((char*) res->next->prev->value, "test1") == 0, 
        "Result of merging NULL and not NULL is just non-NULL nodes"
    );
    
    /* General case */
    List_push(list2, "test2");
    List_push(list2, "test4");
    
    List* test = List_create();
    test->first = list1->first;
    test->last  = list1->last;
    test->count = 5;

    res = ListNode_merge(list1->first, list2->first, (List_compare) strcmp);

    char *values[] = { "test1", "test2", "test3", "test4", "test5" };
    mu_assert(check_consistency(test, values, (List_compare) strcmp) == 0,
              "Given after ListNode_merge list is wrong.");

    /* Cleanup */
    free(list1);
    free(list2);
    List_destroy(test);

    return NULL;
}

char *merge_sort_bottomup_test()
{
    mu_assert(
        List_merge_sort_bottomup(NULL, (List_compare) strcmp) == NULL, 
        "Should correctly works with NULL"
    );

    List *words = create_words();
    
    // should work on a list that needs sorting
    List *res = List_merge_sort_bottomup(words, (List_compare) strcmp);
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

    List *res2 = List_merge_sort_bottomup(res, (List_compare) strcmp);
    mu_assert(is_sorted(res2), "Should still be sorted after merge sort.");

    // List_destroy(words);
    return NULL;
}

char *quick_sort_test()
{
    List *list = List_create();
    List *res = List_quick_sort(list, (List_compare) strcmp);
    mu_assert(res != NULL && res->first == NULL, 
              "Result of sorting empty list should be empty list");

    List_push(list, "test1");
    List_push(list, "test3");
    List_push(list, "test2");
    List_push(list, "test4");

    char *values[] = { "test1", "test2", "test3", "test4" };
    res = List_quick_sort(list, (List_compare) strcmp);
    mu_assert(check_consistency(list, values, (List_compare) strcmp), "Faild to Quick Sort.");
    
    // List_destroy(res);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_swap_nodes);
    mu_run_test(test_bubble_sort);
    mu_run_test(test_bubble_sort_opt);
    mu_run_test(merge_sort_base_test);
    mu_run_test(merge_test);
    mu_run_test(merge_sort_test);
    mu_run_test(listnode_merge_test);
    mu_run_test(merge_sort_bottomup_test);
    mu_run_test(quick_sort_test);

    return NULL;
}

RUN_TESTS(all_tests);
