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

int List_bubble_sort_opt(List *list, List_compare fn)
{
    check_mem(list);

    int i = list->count;
    if (i <= 1) return 0;

    int j = 0;
    int cmp = 0;
    int rc = 0;

    for (; i > 0; i--) {
        ListNode *node     = list->first;
        ListNode *node_max = list->first;
        
        for (j = i - 1; j > 0; j--) {
            node = node->next;
            cmp = fn(node->value, node_max->value);
            if (cmp > 0) { /* this node is bigger than max */
                node_max = node;
            }
        }

        if (node != node_max) {
            rc = swap_nodes(node_max, node); /* now max value at the end */
            check(rc == 0, "Fail during swap_nodes.");
        }

    }
    
    return 0;
error:
    return 1;
}

List *List_merge(List *left, List *right, List_compare fn)
{
    /*
     * function merge(left, right) is
     * var result := empty list

     *   while left is not empty and right is not empty do
     *     if first(left) ≤ first(right) then
     *         append first(left) to result
     *         left := rest(left)
     *     else
     *         append first(right) to result
     *         right := rest(right)

     * // Either left or right may have elements left; consume them.
     * // (Only one of the following loops will actually be entered.)
     * while left is not empty do
     *     append first(left) to result
     *     left := rest(left)
     * while right is not empty do
     *     append first(right) to result
     *     right := rest(right)
     * return result
    */

    int cmp = 0;

    List *result = List_create();
    check_mem(result);

    ListNode *left_node  = left->first;
    ListNode *right_node = right->first;

    while (left_node != NULL && right_node != NULL) {
        cmp = fn(left_node->value, right_node->value);
        if (cmp > 0) { /* means left >= right */
            List_push(result, right_node->value);
            right_node = right_node->next;
        } else {
            List_push(result, left_node->value);
            left_node = left_node->next;
        }
    }

    /* push remaining left_nodes */
    while (left_node != NULL) {
        List_push(result, left_node->value);
        left_node = left_node->next;
    }

    /* push remaining right_nodes */
    while (right_node != NULL) {
        List_push(result, right_node->value);
        right_node = right_node->next;
    }

    return result;

error:
    return NULL;
}

List *List_merge_sort(List *list, List_compare fn)
{
    check_mem(list);

    /* Base Case */
    if (list->count <= 1) return list;
    
    /* Recursive Case */
    List *list_left = list;
    List *list_right = List_split(list_left);

    check_mem(list_left);
    check_mem(list_right);

    List *sorted_left  = List_merge_sort(list_left, fn);
    List *sorted_right = List_merge_sort(list_right, fn);

    List *result = List_join_sorted(sorted_left, sorted_right, fn);

    return result;
error:
    return NULL;
}
