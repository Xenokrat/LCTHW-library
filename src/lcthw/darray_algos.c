#include "darray_algos.h"
#include "darray.h"
#include "dbg.h"
#include <lcthw/darray_algos.h>
#include <stdlib.h>

#define MAX(A, B) (cmp((A), (B)) > 0 ? (A) : (B))

int cmp_int (const void *a, const void *b)
{
    return *(int *) a - *(int *) b;
}

/* Heap sort bullshit */
static inline int ind_left_child(int ind)
{
    return 2 * ind + 1;
}

static inline int ind_parent(int ind)
{
    return (ind - 1) / 2;
}

static inline void swap(void **a, void **b)
{
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

int DArray_partition_qs(DArray *array, int low, int high, DArray_compare cmp)
{
    void **data = array->contents;
    int L = low;
    int H = high;
    void *pivot = data[L];

	while (L < H) {
    	while (cmp(&data[L], &pivot) <= 0 && L <= high - 1)
        	L++;
    	while (cmp(&pivot, &data[H]) <  0 && H >= low + 1)
        	H--;

    	if (L < H)
        	swap(&data[L], &data[H]);
	}

	swap(&data[low], &data[H]);
    return H;
}

int _DArray_qsort(DArray *array, int low, int high, DArray_compare cmp)
{
    check_mem(array);

    /* Recursive Case */
    if (low < high) {
        int pivot_ind = DArray_partition_qs(array, low, high, cmp);

        _DArray_qsort(array, low, pivot_ind - 1, cmp);
        _DArray_qsort(array, pivot_ind + 1, high, cmp);
    }

    return 0;

error:
    return -1;
}

int DArray_qsort(DArray *array, DArray_compare cmp)
{
    check_mem(array);
    if (DArray_count(array) <= 1) {
        return 0;
    }
    int rc = _DArray_qsort(array, 0, array->end - 1, cmp);
    return rc;

error:
    return -1;
}

int DArray_qsort_lib(DArray *array, DArray_compare cmp)
{
    check_mem(array);
	qsort(array->contents, DArray_count(array), sizeof(void *), cmp);

	return 0;
error:
    return -1;
}

/* Heap Sort */
static void siftDown(DArray         *array, 
                     DArray_compare  cmp, 
                     int             root, 
                     int             end)
{
    int child = 0;
    void **arr = array->contents;

    while (ind_left_child(root) < end) {
		child = ind_left_child(root);

		/* If there is a right child and that child is greater */
		if (child + 1 < end && 
		    cmp(&arr[child], &arr[child + 1]) < 0)
    			child++; /* child now right */

    	if (cmp(&arr[root], &arr[child]) < 0) {
        	swap(&arr[root], &arr[child]);
        	root = child; /* repeat to continue sifting down the child now */
    	} else {
        	return;
    	}
    }
}

static void heapify(DArray *array, DArray_compare cmp, size_t index)
{
    /* start is initialized to the first leaf node
     * the last element in a 0-based array is at index count-1;
     * find the parent of that element */
	int start = ind_parent(array->end - 1) + 1;

	while (start > 0) {
    	start--;
    	siftDown(array, cmp, start, array->end);
	}
}

int DArray_heapsort(DArray *array, DArray_compare cmp)
{
    heapify(array, cmp, array->end);

    int end = array->end;
    while (end > 1) {
        end--;
        swap(&array->contents[end], &array->contents[0]);
        siftDown(array, cmp, 0, end);
    }
    return 0;
}

/* Merge Sort */
DArray* merge(DArray *arr1, DArray *arr2, DArray_compare fn)
{
    check(arr1 != NULL, "arr1 is NULL in merge!");
    check(arr2 != NULL, "arr2 is NULL in merge!");

    int cmp = 0;
    int len1 = DArray_count(arr1);
    int len2 = DArray_count(arr2);
    int L = 0;
    int R = 0;
    DArray *result = DArray_create(0, len1 + len2, DArray_expand);

    while (L < len1 && R < len2) {
        cmp = fn(&arr1->contents[L], &arr2->contents[R]);
        if (cmp > 0) {
    		DArray_push(result, arr2->contents[R]);
    		R++;
        } else {
    		DArray_push(result, arr1->contents[L]);
    		L++;
        }
    }

    while (L < len1) {
        DArray_push(result, arr1->contents[L]);
        L++;
    }

    while (R < len2) {
        DArray_push(result, arr2->contents[R]);
        R++;
    }

	DArray_destroy(arr1);
	DArray_destroy(arr2);

    return result;
error:
    return NULL;
}

DArray *_DArray_mergesort(DArray *array, DArray_compare cmp)
{
    check_mem(array);

    int mid = 0;
    int i = 0;

    if (DArray_count(array) <= 1)
        return array;

    DArray *left  = DArray_create(0, DArray_count(array), DArray_expand);
    DArray *right = DArray_create(0, DArray_count(array), DArray_expand);

    mid = array->end / 2 - 1;

    for (i = 0; i < DArray_count(array); i++) {
        if (i <= mid) {
            DArray_push(left, DArray_get(array, i));
        } else {
            DArray_push(right, DArray_get(array, i));
        }
    }

    DArray *left_sorted  = _DArray_mergesort(left, cmp);
    DArray *right_sorted = _DArray_mergesort(right, cmp);
    DArray *result = merge(left_sorted, right_sorted, cmp);

    return result;

error:
    return NULL;
}

int DArray_mergesort(DArray *array, DArray_compare cmp)
{
    check_mem(array);
    DArray *result = _DArray_mergesort(array, cmp);
    check(result != NULL, "Got nullptr from mergesort.");

	array->contents = result->contents;
	DArray_free(result);

    return 0;
error:
    return -1;
}

int DArray_sort_add(DArray         *array,
                    void           *el,
                    DArray_sort_fn  fn,
                    DArray_compare  cmp)
{
  int rc = DArray_push(array, el);
  check(rc == 0, "Element addition failed.");
  
  rc = fn(array, cmp);
  check(rc == 0, "Sorting failed.");
  
  return 0;

error:
  return -1;
}

int DArray_find(DArray *array, void *el, DArray_compare cmp)
{
  int low = 0;
  int high = array->end - 1;
  int cmp_res = 0;
  
  while (low <= high) {
    int middle = (low + high) / 2;
    cmp_res = cmp(&el, &array->contents[middle]);
    
    if (cmp_res > 0)
      low = middle + 1;
    else if (cmp_res < 0)
      high = middle - 1;
    else
      return middle;
    
  }

  return -1;
}


