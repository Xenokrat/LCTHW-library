#include "darray.h"
#include "dbg.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

DArray *DArray_create(size_t element_size, size_t initial_max,
                      DArray_expand_strategy fn)
{
    DArray *array = malloc(sizeof(DArray));
    check_mem(array); 

    array->max = initial_max;
    check(array->max > 0, "You must set initial max > 0");

    array->contents = calloc(initial_max, sizeof(void *)); 
    check_mem(array->contents);

    array->end = 0;
    array->element_size = element_size;
    // check(element_size >= 1, "Element size should be at least 1");

    array->expand_rate = DEFAULT_EXPAND_RATE;
    array->fn = fn;

    return array;

error:
    if (array)
        free(array);
    return NULL;
}

void DArray_clear(DArray *array)
{
    size_t i = 0;
    if (array->element_size <= 0) return;

    for (i = 0; i < array->max; i++) {
        if (array->contents[i]) {
            free(array->contents[i]);
        }
    }
}

static inline int DArray_resize(DArray *array, size_t newsize)
{
    check_mem(array);

    array->max = newsize;
    check(array->max > 0, "The newsize must be > 0.");
    void *contents = realloc(array->contents, array->max * sizeof(void *));
    // check contents and assume realloc doesen't harm the original on error

    check_mem(contents);

    array->contents = contents;

    return 0;
error:
    return -1;
}

int DArray_expand(DArray *array)
{
    check_mem(array); 

    size_t old_max = array->max;
    check(DArray_resize(array, array->max + array->expand_rate) == 0,
          "Failed to expand array to new size: %d",
    	  array->max + (int)array->expand_rate);

    memset(array->contents + old_max, 0, array->expand_rate + 1);
    return 0;

error:
    return -1;
}

int DArray_expand_mul(DArray *array)
{
    check_mem(array); 

    size_t old_max = array->max;
    check(DArray_resize(array, array->max * 2) == 0,
          "Failed to expand array to new size: %d",
    	  array->max * 2);

    memset(array->contents + old_max, 0, old_max);
    return 0;

error:
    return -1;
}

int DArray_contract(DArray *array)
{
    int new_size = array->end < (int)array->expand_rate ?
    	(int)array->expand_rate : array->end;

    return DArray_resize(array, new_size + 1);
}

void DArray_destroy(DArray *array)
{
    if (array) {
        if (array->contents)
            free(array->contents);
        free(array);
    }
}

void DArray_clear_destroy(DArray *array)
{
    DArray_clear(array); 
    DArray_destroy(array);
}

int DArray_push(DArray *array, void *el)
{
    check_mem(array);
    array->contents[array->end] = el;
    array->end++;

    if (DArray_end(array) >= DArray_max(array)) {
        return array->fn(array);
    } else {
        return 0;
    }
error:
    return -1;
}

int DArray_insert(DArray *array, unsigned position, void *el)
{
    int rc = 0;
    size_t i = 0;

    check_mem(array);
    check(position <= array->end, "Cannot insert past end of the array.");

    array->end++;

    if (DArray_end(array) >= DArray_max(array)) {
        rc = array->fn(array);
        check(rc != 0, "Failed to expand array on insert.");
    }
    for (i = array->end - 1; i > position; i--) {
        DArray_set(array, i, DArray_get(array, i - 1));
    }
    DArray_set(array, position, el);

    return 0;
error:
    return -1;
}

void *DArray_pop(DArray *array)
{
    check(array->end - 1 > 0, "Attempt to pop from empty array.");

    void *el = DArray_remove(array, array->end - 1);
    array->end--;

    if (DArray_end(array) > (int)array->expand_rate &&
        DArray_end(array) % array->expand_rate) {
            DArray_contract(array);
    }

    return el;
error:
    return NULL;
}


