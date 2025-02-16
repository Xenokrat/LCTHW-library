#include "list_algos.h"
#include "dbg.h"

#define UNUSED(x) (void)(x)

int swap_nodes(ListNode *node1, ListNode *node2)
{
    check_mem(node1);
    check_mem(node2);
    check(node1 != node2, "Swapping the same node!");

    void *tmp = node1->value;
    node1->value = node2->value;
    node2->value = tmp;

    return 0;
error:
    return 1;
}

int List_bubble_sort(List *list, List_compare fn)
{
   /*
    * procedure bubbleSort(A : list of sortable items)
    *     n := length(A)
    *     repeat
    *         swapped := false
    *         for i := 1 to n-1 inclusive do
    *             { if this pair is out of order }
    *             if A[i-1] > A[i] then
    *                 { swap them and remember something changed }
    *                 swap(A[i-1], A[i])
    *                 swapped := true
    *             end if
    *         end for
    *     until not swapped
    * end procedure
    */

    int rc = 0;
    int cmp = 0;
    int swapped = 0;
    int len = list->count;

    /* Empty or 1-element list case */
    if (len <= 1) return 0;

    ListNode *node = list->first;
    ListNode *node_next = node->next;

    do {
        swapped = 0;
        while (node_next != NULL) {
            cmp = fn(node->value, node_next->value);
            if (cmp > 0) {      /* > 0 => node->value > node_next->value and we should swap */
                rc = swap_nodes(node, node_next);
                check(rc == 0, "Fail during swap_nodes.");
                swapped = 1;
                node_next = node->next; 
            } else {
                node = node_next;
                node_next = node_next->next;
            }
        }
        node = list->first;
        node_next = node->next;
    } while (swapped);

    return 0;
error:
    return 1;
}


List *List_merge_sort(List *list, List_compare fn)
{
    /*
    function merge_sort(list m) is
    // Base case. A list of zero or one elements is sorted, by definition.
    if length of m ≤ 1 then
        return m

    // Recursive case. First, divide the list into equal-sized sublists
    // consisting of the first half and second half of the list.
    // This assumes lists start at index 0.
    var left := empty list
    var right := empty list
    for each x with index i in m do
        if i < (length of m)/2 then
            add x to left
        else
            add x to right

    // Recursively sort both sublists.
    left := merge_sort(left)
    right := merge_sort(right)

    // Then merge the now-sorted sublists.
    
    return merge(left, right)
    */

    /* Base Case */
    if (list->count <= 1) return list;
    
    int middle = list->count / 2;

    /* Recursive Case */
    List *list_left  = List_create();
    List *list_right = List_create();

    return NULL;
}
