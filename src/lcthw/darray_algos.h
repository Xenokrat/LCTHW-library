#ifndef darray_algos_h
#define darray_algos_h

#include <lcthw/darray.h>

typedef int (*DArray_compare) (const void *a, const void *b);

typedef int (*DArray_sort_fn) (DArray *array, DArray_compare cmp);

int cmp_int (const void *a, const void *b);

int DArray_qsort(DArray *array, DArray_compare cmp);

int DArray_qsort_lib(DArray *array, DArray_compare cmp);

int DArray_heapsort(DArray *array, DArray_compare cmp);

int DArray_mergesort(DArray *array, DArray_compare cmp);

int DArray_sort_add(DArray *array, void* el, DArray_sort_fn fn,
                    DArray_compare cmp);

int DArray_find(DArray *array, void* el, DArray_compare cmp);

#endif // darray_algos_h
