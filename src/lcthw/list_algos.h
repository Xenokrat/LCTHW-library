#ifndef	lcthw_List_Algos_h
#define	lcthw_List_Algos_h

#include "list.h"

int swap_nodes(ListNode *node1, ListNode *node2);

/* Sorts List type with `bubble sort` method
 * return 0 if OK, else 1
 */
int List_bubble_sort(List *list, List_compare fn);

/* Optimized version of `bubble_sort`
 * return 0 if OK, else 1 */
int List_bubble_sort_opt(List *list, List_compare fn);

/* Sorts List type with `merge sort` method
 * returns pointer to new sorted list (NEEDS FREE!) */
List *List_merge(List *left, List *right, List_compare fn);
List *List_merge_sort(List *list, List_compare fn);

#endif	// lcthw_List_Algos_h
