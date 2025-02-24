#ifndef lcthw_List_h
#define lcthw_List_h

#include <stdlib.h>

typedef int (*List_compare) (void *value1, void *value2);
int intcmp(void *value1, void *value2);

struct ListNode;

/* Node is instance of node in Double linked-list data structure */
typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    void *value;
} ListNode;

/* List is a container for ListNode structs
 * it has a pointer to first element in List
 * and last element in list
 */
typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
} List;

/* we will use this to identify that values in Node are equal
 * return 1 if so, else 0
 */
typedef int (*Fn_node_eq) (void *value1, void *value2);
int str_node_eq(void *value1, void *value2); /* compare str values */

List *List_create(void);                       /* Allocate new List in heap */
void List_destroy(List *list);                 /* Deallocate all ListNode and free List */
void List_clear(List *list);                   /* Deallocate all values in ListNodes */
void List_clear_destroy(List *list);           /* Free all values in ListNodes, free ListNodes, free List */

void List_print_str(List *list);           /* This just a helper function to print list to stdout */
void ListNode_print_str(ListNode *node);   /* This just a helper function to print from node to stdout */

/* Macros for easy access to count, first and last ListNode in List */
#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A)  ((A)->last != NULL ? (A)->last->value : NULL)

void List_push(List *list, void *value);       /* push new ListNode with `value` to the end of List */
void *List_pop(List *list);                    /* pop last ListNode from the end of List and return it's `value` */

void List_unshift(List *list, void *value);    /* push new ListNode with `value to the start of List */
void *List_shift(List *list);                  /* pop last ListNode from start of List and return it's `value` */ 
ListNode *List_shift_get(List *list);          /* same as List_shif but returns node instead */ 

void *List_remove(List *list, ListNode *node); /* removes ListNode from List and frees it */

ListNode *List_find(List *list, void *value, Fn_node_eq fn);  /* return pointer to ListNode with value `value from `list` */

List *List_copy(List *list);                   /* create new List which is copy of `list` and returns pointer to it */
List *List_join(List *list1, List *list2);     /* re-use list1 adding nodes of list2 to it and free list2, return pointer to list1 */

/* pre-condition: list1 and list2 are soreted
 * re-using list1 (or list2) to join all nodes in sorted order */
List *List_join_sorted(List *list1, List *list2, List_compare fn);     

/* split list in two
 * used for algorithms like sorting
 * modify list argument in-place, returns pointer to new second list
 * ( node1 -> node2 -> node3 ) =(List_split)> ( node1 -> node2 ) -> ( node3 )
 * (  ) =(List_split)> (  )
 * ( node1  ) =(List_split)> ( node1 ) ( ) */
List *List_split(List *list);  

/* Inserts `value` in already sorted `list` at right position so it
 * remains sorted. Using `fn` to compare elements */
void List_insert_sorted(List *list, void *value, List_compare fn);  

/* Iterator for traversing the List struct
 * L - list
 * S - start point (list->first)
 * M - next Node (node->next)
 * V - node value to use in loop body
 * 
 * Example:
 * 	LIST_FOREACH(list, first, next, cur) {
 *           printf(cur->value);
 * 	} */
#define LIST_FOREACH(L, S, M, V) \
    ListNode *_node = NULL; \
    ListNode *V = NULL; \
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)

/* Helper function to check that we are not corrupted List */
int check_consistency(List *list, char *values[], List_compare fn);

#endif // lcthw_List_h 
