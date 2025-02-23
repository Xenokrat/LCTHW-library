#include "list_algos.h"
#include "dbg.h"
#include "list.h"

#define UNUSED(x) (void)(x)
#define ARR_SIZE 32

int is_sorted(List* list)
{
    if (list == NULL) return 0;

    LIST_FOREACH(list, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            debug("%s %s", (char *) cur->value,
                           (char *) cur->next->value);
            return 0;
        }
    }

    return 1;
}

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

    if (is_sorted(list) == 1) {
        debug("List is actually sorted");
    } else {
        log_err("List IS NOT sorted!");
    }

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
    
    if (is_sorted(list) == 1) {
        debug("List is actually sorted");
    } else {
        log_err("List IS NOT sorted!");
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

    if (is_sorted(result) == 1) {
        debug("List is actually sorted");
    } else {
        log_err("List IS NOT sorted!");
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

static inline void ListNode_clear(ListNode *node)
{
    node->prev = NULL;
    node->next = NULL;
}

static inline void ListNode_append(ListNode *node1, ListNode *node2)
{
    if (!node1) return;
    if (!node2) return;

    ListNode_clear(node2);
    node1->next = node2;
    node2->prev = node1;
}

ListNode *ListNode_merge(ListNode *node1, ListNode *node2, List_compare fn)
{
    int cmp         = 0;
    ListNode *first = NULL;
    ListNode *cur   = NULL;
    ListNode *tmp   = NULL;

    if (node1 == NULL && node2 == NULL)
        return NULL;    
    
    if (node1 != NULL && node2 == NULL)
        return node1;
    
    if (node1 == NULL && node2 != NULL)
        return node2;

    cmp = fn(node1->value, node2->value);
    if (cmp > 0) {
        first = node2;
        node2 = node2->next;
    } else {
        first = node1;
        node1 = node1->next;
    }
    ListNode_clear(first);
    cur = first;

    while (node1 != NULL && node2 != NULL) {
        cmp = fn(node1->value, node2->value);
        if (cmp > 0) {
            tmp = node2->next;
            ListNode_append(cur, node2);
            cur = node2;
            node2 = tmp;
        } else {
            tmp = node1->next;
            ListNode_append(cur, node1);
            cur = node1;
            node1 = tmp;
        }
    }
    
    while (node1 != NULL) {
        ListNode_append(cur, node1);
        cur = node1;
        node1 = node1->next;
    }
    
    while (node2 != NULL) {
        ListNode_append(cur, node2);
        cur = node2;
        node2 = node2->next;
    }

    return first;
}

List *List_merge_sort_bottomup(List *list, List_compare fn)
{
    /*
     * function merge_sort(node head) is
     * // return if empty list
     * if head = nil then
     *     return nil
     * var node array[32]; initially all nil
     * var node result
     * var node next
     * var int  i
     * result := head
     * // merge nodes into array
     * while result ≠ nil do
     *     next := result.next;
     *     result.next := nil
     *     for (i = 0; (i < 32) && (array[i] ≠ nil); i += 1) do
     *         result := merge(array[i], result)
     *         array[i] := nil
     *     // do not go past end of array
     *     if i = 32 then
     *         i -= 1
     *     array[i] := result
     *     result := next
     * // merge array into single list
     * result := nil
     * for (i = 0; i < 32; i += 1) do
     *     result := merge(array[i], result)
     * return result
     */

    check_mem(list);

    ListNode *head = list->first;
    if (head == NULL)
        return NULL;
    
    ListNode *array[ARR_SIZE] = { NULL };
    ListNode *result          = NULL;
    ListNode *next            = NULL;
    int i = 0;

    result = head;

    /* merge nodes into array */
    while (result != NULL) {
        next = result->next;
        ListNode_clear(result);
        
        for (i = 0; i < ARR_SIZE && array[i] != NULL; i++) {
            result = ListNode_merge(array[i], result, fn);
            array[i] = NULL;
        }
        /* Do not go past end of array ??? */
        if (i == ARR_SIZE) 
            i--;
        array[i] = result;
        result = next;
    }
    /* merge array into single list */
    result = NULL;
    for (i = 0; i < ARR_SIZE; i++) {
        result = ListNode_merge(array[i], result, fn);
    }

    List *list_res = List_create();
    list_res->first = result;
    
    /* I'm sure hope that result here CANNOT be NULL */
    i = 1;
    while (result->next != NULL) {
        result = result->next;
        i++;
    }
    list_res->last = result;
    list_res->count = i;

    return list_res;
error:
    return NULL;
}

void qs_partitioning(ListNode *pivot,
                     ListNode **left_end,
                     ListNode **left_start,
                     ListNode **right_start,
                     List_compare fn)
{
    int cmp = 0;

    if (pivot == NULL)
        return;

    ListNode *cur   = pivot->next;
    ListNode *tmp   = NULL;
    ListNode *left  = NULL;
    ListNode *right = NULL;

    if (cur == NULL)
        return;

    while (cur) {
        tmp = cur->next;
        cmp = fn((char*) pivot->value, (char*) cur->value);
        ListNode_clear(cur);

        if (cmp > 0) {
            if (left) {
                ListNode_append(left, cur);
                left = left->next;
            } else {
                left = cur;
                *left_start = cur;
            }
        } else {
            if (right) {
                ListNode_append(right, cur);
                right = right->next;
            } else {
                right = cur;
                *right_start = cur;
            }
        }
        cur = tmp;
    }

    /* NOTE: i fucking DON'T know what I'm doing */
    if (!left) *left_start = NULL;
    if (!right) *right_start = NULL;

    *left_end = left;
}

ListNode *ListNode_qs(ListNode *node, List_compare fn)
{
    if (node == NULL || node->next == NULL)
        return node;

    ListNode *left_end;
    ListNode *left_start;
    ListNode *right_start;

    /* God this is ugly */
    qs_partitioning(
        node,
        &left_end,
        &left_start,
        &right_start,
        fn
    );

    ListNode_qs(left_start, fn);
    ListNode_append(left_end, node);
    ListNode_append(node, ListNode_qs(right_start, fn));

error:
    return NULL;
}

List *List_quick_sort(List *list, List_compare fn)
{
    check_mem(list);
    int i = 0;
    
    ListNode *node = list->first;
    if (node == NULL || node->next == NULL)
        return list;
    
    ListNode *result = ListNode_qs(node, fn);

    /* Should abstract that */
    List *list_res = List_create();
    i = 0;
    while (result != NULL) {
        result = result->next;
        i++;
    }
    list_res->last = result;
    list_res->count = i;
    return list_res;

error:
    return NULL;
}
