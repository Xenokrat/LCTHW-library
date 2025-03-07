#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <lcthw/list.h>
#include <lcthw/list_algos.h>
#include <lcthw/darray.h>
#include <lcthw/dbg.h>

#define TEST_ARR_SIZE 10000
#define TEST_ARR_MAX 1000

List* list = NULL;
DArray* array = NULL;

static inline int *
create_random_int_array(size_t size, int max_value)
{
    size_t i = 0;
    int *arr = malloc(sizeof(int) * size);
    check_mem(arr);

    for (i = 0; i < size; i++) {
    	arr[i] = rand() % max_value;
    }
    return arr;

error:
    return NULL;
}

// setup
int setUp(void)
{
    size_t i = 0;
    list = List_create();
    array = DArray_create(sizeof(int), 100, (DArray_expand_strategy) DArray_expand);

    check_mem(list);
    check_mem(array);

    int *list_arr = create_random_int_array(TEST_ARR_SIZE, TEST_ARR_MAX);

    /* setup list */
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        /* NOTE: LUL */
        int *el = (int *) DArray_new(array); 
        *el = list_arr[i];
        List_push(list, el);
    }
    /* setup darray */
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *el = (int *) DArray_new(array); 
        *el = list_arr[i];
        DArray_push(array, el);
    }

    free(list_arr);
    return 0;
error:
    return -1;
}

// Test push
void test_push()
{
    float start_time = 0.0;
    float end_time = 0.0;
    size_t i = 0;

    /* Duplicates setUp but whatever */
    List *push_list    = List_create();
    DArray *push_array = DArray_create(sizeof(int), 100, (DArray_expand_strategy) DArray_expand);
    int *int_arr = create_random_int_array(TEST_ARR_SIZE, TEST_ARR_MAX); 

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *value = (int *) DArray_new(push_array);
        *value = int_arr[i];
    	List_push(push_list, value);
    }
    end_time = (float)clock() / CLOCKS_PER_SEC; 
    printf("List_push time for inserting %d elements is %f\n", 
           TEST_ARR_SIZE,
           end_time - start_time);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *value = (int *) DArray_new(push_array);
        *value = int_arr[i];
    	DArray_push(push_array, value);
    }
    end_time = (float)clock() / CLOCKS_PER_SEC; 
    printf("DArray_push time is %f\n", end_time - start_time);

    /* Cleanup */
    free(int_arr);
    List_clear_destroy(push_list);
    DArray_clear_destroy(push_array);
}

// Test insertion
void test_insertion(void)
{
    const unsigned arr_size = TEST_ARR_SIZE / 4;
    int *int_ins_val_arr = create_random_int_array(arr_size, TEST_ARR_MAX);
    int *int_ins_pos_arr = create_random_int_array(arr_size, arr_size - 1);

    float start_time = 0.0;
    float end_time = 0.0;
    size_t i = 0;
    unsigned pos = 0;

    /* test List */
    /* Need sorted List to insert */
    List *list_copy = List_copy(list);
    List_quick_sort2(list_copy, intcmp);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < arr_size; i++) {
        int *value = (int *) DArray_new(array);
        *value = int_ins_val_arr[i];
    	List_insert_sorted(list_copy, value, intcmp);
    }
    end_time   = (float)clock() / CLOCKS_PER_SEC; 
    printf("List time for inserting %d elements is %f\n", 
           arr_size,
           end_time - start_time);

    /* test DArray */
    DArray *array_ins = DArray_create(sizeof(int), TEST_ARR_SIZE,
                                      (DArray_expand_strategy) DArray_expand);
    int *int_arr = create_random_int_array(TEST_ARR_SIZE, TEST_ARR_MAX); 
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *value = (int *) DArray_new(array_ins);
        *value = int_arr[i];
    	DArray_push(array_ins, value);
    }

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < arr_size; i++) {
        int *value = (int *) DArray_new(array);
        *value = int_ins_val_arr[i];
        pos    = int_ins_pos_arr[i];  
    	DArray_insert(array_ins, pos, value);
    }
    end_time   = (float)clock() / CLOCKS_PER_SEC; 
    printf("DArray_insert time for inserting %d elements is %f\n", 
           arr_size,
           end_time - start_time);

    /* Clean Up */
    /* List_clear_destroy(list_copy); WHY??? */
    DArray_clear_destroy(array_ins);
    free(int_ins_pos_arr);
    free(int_ins_val_arr);
    free(int_arr);
}

void test_expand(void)
{
    float start_time = 0.0;
    float end_time = 0.0;
    size_t i = 0;

    DArray *darr1 = DArray_create(sizeof(int),
                                  100, 
                                  (DArray_expand_strategy) DArray_expand);

    DArray *darr2 = DArray_create(sizeof(int),
                                  100, 
                                  (DArray_expand_strategy) DArray_expand_mul);

    int *arr_of_int = create_random_int_array(TEST_ARR_SIZE, TEST_ARR_MAX);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *el = (int *) DArray_new(array); 
        *el = arr_of_int[i];
        DArray_push(darr1, el);
    }
    end_time = (float)clock() / CLOCKS_PER_SEC; 
    printf("DArray_push time for +300 strategy is %f\n", 
           end_time - start_time);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    for (i = 0; i < TEST_ARR_SIZE; i++) {
        int *el = (int *) DArray_new(array); 
        *el = arr_of_int[i];
        DArray_push(darr2, el);
    }
    end_time = (float)clock() / CLOCKS_PER_SEC; 
    printf("DArray_push time for *2 strategy is %f\n", 
           end_time - start_time);

    /* Cleanup */
    free(arr_of_int);
    DArray_clear_destroy(darr1);
    DArray_clear_destroy(darr2);
}

// Test deletion

void tearDown(void)
{
    if (list)
        List_clear_destroy(list);
    if (array)
        DArray_clear_destroy(array);
}

int main() 
{
    printf("hello from performance tests!\n");
    setUp();

    /* tests */
    printf("\n=== TEST PUSH PERFORMANCE ===\n");
    test_push();

    printf("\n=== TEST INSERT PERFORMANCE ===\n");
    test_insertion();

    printf("\n=== TEST DARRAY +300 vs *2 EXPAND  ===\n");
    test_expand();

    tearDown();
    return 0;
}
