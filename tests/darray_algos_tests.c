#include "minunit.h"
#include <lcthw/darray_algos.h>
#include <stdlib.h>

#define ARRAY_SIZE(A) (sizeof((A)) / sizeof((A[0])))

int testcmp(char **a, char **b)
{
    return strcmp(*a, *b);
}

DArray* create_words()
{
    DArray *result = DArray_create(0, 5, DArray_expand);
    // char *words[] = { "asdfasfd",
    //     "werwar", "13234", "asdfasfd", "oioj" };
    char *words[] = { "3", "1", "5", "4", "2" };
    int i = 0;

    for (i = 0; i < 5; i++) {
        DArray_push(result, words[i]);
    }

    return result;
}

int is_sorted(DArray *array)
{
    int i = 0;

    for (i = 0; i < DArray_count(array) - 1; i++) {
        if (strcmp(DArray_get(array, i), DArray_get(array, i + 1)) > 0)
            return 0;
    }

    return 1;
}

char *run_sort_test(int (*func) (DArray *, DArray_compare),
                    const char *name)
{
    DArray *words = create_words();
    mu_assert(!is_sorted(words), "Words should start not sorted!");

    debug("--- Testing %s sorting algorithm", name);
    int rc = func(words, (DArray_compare) testcmp); 
    mu_assert(rc == 0, "sort failed.");
    mu_assert(is_sorted(words), "didn't sort array.");

    mu_assert(strcmp(((char*) words->contents[0]), "1") == 0, "Failed to sort");
    mu_assert(strcmp(((char*) words->contents[1]), "2") == 0, "Failed to sort");
    mu_assert(strcmp(((char*) words->contents[2]), "3") == 0, "Failed to sort");
    mu_assert(strcmp(((char*) words->contents[3]), "4") == 0, "Failed to sort");
    mu_assert(strcmp(((char*) words->contents[4]), "5") == 0, "Failed to sort");

    DArray_destroy(words);

    return NULL;
}

char *test_qsort()
{
    return run_sort_test(DArray_qsort, "qsort");
}

char *test_heapsort()
{
    return run_sort_test(DArray_heapsort, "heapsort");
}

char *test_mergesort()
{
    return run_sort_test(DArray_mergesort, "mergesort");
}

char *test_sort_add()
{
    DArray *words = create_words();
    mu_assert(!is_sorted(words), "Words should start not sorted!");

    int rc = DArray_qsort(words, (DArray_compare) testcmp); 
    mu_assert(rc == 0, "sort failed.");
    mu_assert(is_sorted(words), "didn't sort array.");

    DArray_sort_add(words, "6", DArray_qsort, (DArray_compare) testcmp);
    DArray_sort_add(words, "9", DArray_qsort, (DArray_compare) testcmp);
    DArray_sort_add(words, "7", DArray_qsort, (DArray_compare) testcmp);

    mu_assert(rc == 0, "sort failed.");
    mu_assert(is_sorted(words), "didn't sort array.");

    DArray_destroy(words);
  return NULL;
}

char *test_find()
{
    DArray *words = create_words();
    int rc = DArray_mergesort(words, (DArray_compare) testcmp);
    mu_assert(rc == 0, "Qsort failed.");
    char *to_find = "4";
    
    rc = DArray_find(words, (void *) to_find, (DArray_compare) testcmp);
    mu_assert(rc == 3, "Failed to find right index.");
                   
    DArray_destroy(words);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_qsort);
    mu_run_test(test_heapsort);
    mu_run_test(test_mergesort);
    mu_run_test(test_sort_add);
    mu_run_test(test_find);

    return NULL;
}

RUN_TESTS(all_tests);
