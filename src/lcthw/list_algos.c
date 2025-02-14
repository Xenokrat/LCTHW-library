#include "list_algos.h"
#include "dbg.h"

#define UNUSED(x) (void)(x)

int swap_nodes(List* list, ListNode *node1, ListNode *node2)
{
    check_mem(list);
    check_mem(node1);
    check_mem(node2);
    check(node1 != node2, "Swapping the same node!");

    if (list->first == node1) {
        list->first = node2;
    }
    if (list->last == node2) {
        list->last = node1;
    }

    ListNode *node1_prev = node1->prev;
    ListNode *node2_next = node2->next;

    node2->prev = node1_prev;
    node2->next = node1;
    /* Update neighbour nodes */
    if (node1_prev) node1_prev->next = node2;

    node1->next = node2_next;
    node1->prev = node2;
    /* Update neighbour nodes */
    if (node2_next) node2_next->prev = node1;

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

    printf("List before\n");
    List_print_str(list);
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
                rc = swap_nodes(list, node, node_next);
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

    printf("List after\n");
    List_print_str(list);
    printf("\n");

    return 0;
error:
    return 1;
}


List *List_merge_sort(List *list, List_compare fn)
{
    /* TODO: */
    UNUSED(list);
    UNUSED(fn);
    return NULL;
}
