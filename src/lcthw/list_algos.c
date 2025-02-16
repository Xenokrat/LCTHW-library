#include "list_algos.h"
#include "dbg.h"

#define UNUSED(x) (void)(x)

int swap_adjacent_nodes(List* list, ListNode *node1, ListNode *node2);
int swap_non_adjacent_nodes(List* list, ListNode *node1, ListNode *node2);

int swap_nodes(List* list, ListNode *node1, ListNode *node2)
{
    check_mem(list);
    check_mem(node1);
    check_mem(node2);
    check(node1 != node2, "Swapping the same node!");

    int rc = 0;
    
    /* Update List */
    if (node1->prev == NULL) {
        list->first = node2;
    } else if (node2->prev == NULL) {
        list->first = node1;
    }
    
    if (node1->next == NULL) {
        list->last = node2;
    } else if (node2->next == NULL) {
        list->last = node1;
    }


    /* Check if nodes are neighbours */
    if ((node1->next == node2 && node2->prev == node1) ||
        (node1->prev == node2 && node2->next == node1)) {
            rc = swap_adjacent_nodes(list, node1, node2);
        } else {
            rc = swap_non_adjacent_nodes(list, node1, node2);
        }

    if (rc == 0)
        return 0;
error:
    return 1;
}

int swap_adjacent_nodes(List* list, ListNode *node1, ListNode *node2)
{
    ListNode *node_first  = NULL;
    ListNode *node_second = NULL;
    
    if (node1->next == node2) {
        node_first  = node1;
        node_second = node2;
    } else if (node2->next == node1) {
        node_first  = node2;
        node_second = node1;
    } else {
        sentinel("Nodes are not adjacent!");
    }

    ListNode *node_first_prev  = node_first->prev;
    ListNode *node_second_next = node_second->next;

    node_first->next = node_second_next;
    node_second->prev = node_first_prev;
    node_first->prev = node_second;
    node_second->next = node_first;

    if (node_first_prev)  node_first_prev->next  = node_second;
    if (node_second_next) node_second_next->prev = node_first;

    return 0;

error:
    return 1;
}

int swap_non_adjacent_nodes(List* list, ListNode *node1, ListNode *node2)
{
    /* n0 -> [n1] -> n2 -> [n3] -> n4 <=> */
    /* n0 -> [n3] -> n2 -> [n1] -> n4 */
    ListNode *n1_prev = node1->prev;
    ListNode *n1_next = node1->next;
    
    ListNode *n2_prev = node2->prev;
    ListNode *n2_next = node2->next;

    check(n1_prev != node2 && n1_next != node2, "Nodes are adjacnet");
    check(n2_prev != node1 && n2_next != node1, "Nodes are adjacnet");

    /* Update Swapped nodes */
    node1->next = n2_next;
    node1->prev = n2_prev;

    node2->next = n1_next;
    node2->prev = n1_prev;
    
    /* Update Neighbour nodes */
    if (n1_prev) n1_prev->next = node2;
    if (n1_next) n1_next->prev = node2;
    
    if (n2_prev) n2_prev->next = node1;
    if (n2_next) n2_next->prev = node1;
    
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
