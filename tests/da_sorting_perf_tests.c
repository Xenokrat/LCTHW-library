#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include <lcthw/darray_algos.h>
#include <lcthw/radixmap.h>
#include <lcthw/dbg.h>

#define MAX_ARRAY_SIZE 25000

static int uint_cmp(void *val1, void *val2)
{
    return *(uint32_t *) val1 - *(uint32_t *) val2;
}

static uint32_t *create_array()
{
    size_t i = 0;

	uint32_t *array = calloc(MAX_ARRAY_SIZE, sizeof(uint32_t));
	check_mem(array);

    for (i = 0; i < MAX_ARRAY_SIZE; i++) {
        array[i] = (uint32_t) (rand() | (rand() << 16));
    }

    return array;
error:
    return NULL;
}

static uint32_t *copy_array(uint32_t *arr)
{
    size_t i = 0;

	uint32_t *copy = calloc(MAX_ARRAY_SIZE, sizeof(uint32_t));
	check_mem(copy);

    for (i = 0; i < MAX_ARRAY_SIZE; i++) {
        copy[i] = arr[i];
    }

	return copy;
error:
    return NULL;
}

void test_qs_lib(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    DArray *array = DArray_create(0, MAX_ARRAY_SIZE, DArray_expand);
	uint32_t *copy_data = copy_array(data);

	for (i = 0; i < MAX_ARRAY_SIZE; i++) {
    	DArray_push(array, &copy_data[i]);
	}

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    DArray_qsort_lib(array, (DArray_compare) uint_cmp);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("QUICK SORT LIB time for %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    DArray_destroy(array);
    free(copy_data);
}

void test_qs_mine(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    DArray *array = DArray_create(0, MAX_ARRAY_SIZE, DArray_expand);
	uint32_t *copy_data = copy_array(data);

	for (i = 0; i < MAX_ARRAY_SIZE; i++) {
    	DArray_push(array, &copy_data[i]);
	}

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    DArray_qsort(array, (DArray_compare) uint_cmp);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("QUICK SORT MINE time for %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    DArray_destroy(array);
    free(copy_data);
}

void test_heapsort_mine(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    DArray *array = DArray_create(0, MAX_ARRAY_SIZE, DArray_expand);
	uint32_t *copy_data = copy_array(data);

	for (i = 0; i < MAX_ARRAY_SIZE; i++) {
    	DArray_push(array, &copy_data[i]);
	}

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    DArray_heapsort(array, (DArray_compare) uint_cmp);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("HEAP SORT MINE time for %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    DArray_destroy(array);
    free(copy_data);
}

void test_mergesort_mine(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    DArray *array = DArray_create(0, MAX_ARRAY_SIZE, DArray_expand);
	uint32_t *copy_data = copy_array(data);

	for (i = 0; i < MAX_ARRAY_SIZE; i++) {
    	DArray_push(array, &copy_data[i]);
	}

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    DArray_mergesort(array, (DArray_compare) uint_cmp);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("MERGE SORT MINE time for %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    DArray_destroy(array);
    free(copy_data);
}

void test_radix_zed(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    RadixMap *map = RadixMap_create(MAX_ARRAY_SIZE);
	uint32_t *copy_data = copy_array(data);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
	for (i = 0; i < MAX_ARRAY_SIZE - 1; i++) {
    	check(RadixMap_add(map, copy_data[i], i) == 0, "Failed to add into RadixMap.");
	}
    end_time   = (float)clock() / CLOCKS_PER_SEC; 
    printf("RADIX SORT ZED time for ADD %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    RadixMap_sort(map);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("RADIX SORT ZED time for SORT %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    RadixMap_destroy(map);
    free(copy_data);

error:
    return;
}

void test_radix_me(uint32_t *data)
{
    size_t i = 0;
    float start_time = 0.0;
    float end_time   = 0.0;

    RadixMap *map = RadixMap_create(MAX_ARRAY_SIZE);
	uint32_t *copy_data = copy_array(data);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
	for (i = 0; i < MAX_ARRAY_SIZE - 1; i++) {
    	check(RadixMap_add2(map, copy_data[i], i) == 0,
              "Failed to add into RadixMap.");
	}
    end_time   = (float)clock() / CLOCKS_PER_SEC; 
    printf("RADIX SORT ME time for ADD %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    start_time = (float)clock() / CLOCKS_PER_SEC; 
    RadixMap_sort(map);
    end_time   = (float)clock() / CLOCKS_PER_SEC; 

    printf("RADIX SORT ME time for SORT %d elements is %f\n", 
           MAX_ARRAY_SIZE,
           end_time - start_time);

    RadixMap_destroy(map);
    free(copy_data);
error:
  return;
}

int main(void)
{
	uint32_t *array = create_array();

	printf("\n=== TEST QUICK SORT | STDLIB ===\n");
	test_qs_lib(array);

	printf("\n=== TEST QUICK SORT | MINE ===\n");
	test_qs_mine(array);

	printf("\n=== TEST HEAP SORT | MINE ===\n");
	test_heapsort_mine(array);

	printf("\n=== TEST MERGE SORT | MINE ===\n");
	test_mergesort_mine(array);

	printf("\n=== TEST RADIX SORT | ZED ===\n");
	test_radix_zed(array);

   	printf("\n=== TEST RADIX SORT | ME ===\n");
	test_radix_me(array);

	return 0;
}
