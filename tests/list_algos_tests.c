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

int is_sorted(List *words)
{
    LIST_FOREACH(words, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            debug("%s %s", (char *)cur->value,
                           (char *)cur->next->value);
            return 0;
        }
    }

    return 1;
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
    swap_nodes(list, node1, node2);
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
        printf("i: %d, tst[i]: %s, cur->value: %s\n", 
                i, tst[i], (char*) cur->value);
        mu_assert(strcmp((char*) cur->value, tst[i]) == 0,
                  "Fail to swap non-adjacent nodes correctly (forward)");
    }
    
    /* Also tests backwards */
    for (i = tst_len - 1, cur = list->last; 
         cur != NULL && i >= 0; 
         i--, cur = cur->prev) 
    {
        printf("i: %d, tst[i]: %s, cur->value: %s\n", 
                i, tst[i], (char*) cur->value);
        mu_assert(strcmp((char*) cur->value, tst[i]) == 0,
                  "Fail to swap non-adjacent nodes correctly (backward)");
    }
    
    /* Another round for ajustent nodes */
    swap_nodes(list, list->first, list->first->next);
    
    char *tst2[] = { "test2", "test3", "test1", "test4" };
    i = 0;
    cur = NULL;

    /* Test forward again */
    for (i = 0, cur = list->first; 
         cur != NULL && i < tst_len; 
         i++, cur = cur->next) 
    {
        printf("i: %d, tst2[i]: %s, cur->value: %s\n", 
                i, tst2[i], (char*) cur->value);
        mu_assert(strcmp((char*) cur->value, tst2[i]) == 0,
                  "Fail to swap adjacent nodes correctly (forward)");
    }
    
    /* Also tests backwards again */
    for (i = tst_len - 1, cur = list->last; 
         cur != NULL && i >= 0; 
         i--, cur = cur->prev) 
    {
        printf("i: %d, tst2[i]: %s, cur->value: %s\n", 
                i, tst2[i], (char*) cur->value);
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

char *merge_sort_test()
{
    List *words = create_words();

    // should work on a list that needs sorting
    List *res = List_merge_sort(words, (List_compare) strcmp);
    mu_assert(is_sorted(res), "Words are not sorted after merge sort.");

    List *res2 = List_merge_sort(res, (List_compare) strcmp);
    mu_assert(is_sorted(res2),
              "Should still be sorted after merge sort.");
    List_destroy(res2);
    List_destroy(res);

    List_destroy(words);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_swap_nodes);
    mu_run_test(test_bubble_sort);
    // mu_run_test(merge_sort_test);

    return NULL;
}

RUN_TESTS(all_tests);
