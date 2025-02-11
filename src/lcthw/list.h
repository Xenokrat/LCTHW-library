#ifndef lcthw_List_h
#define lcthw_List_h

#include <stdlib.h>

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

/* Macros for easy access to count, first and last ListNode in List */
#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A)  ((A)->last != NULL ? (A)->last->value : NULL)

void List_push(List *list, void *value);       /* push new ListNode with `value` to the end of List */
void *List_pop(List *list);                    /* pop last ListNode from the end of List and return it's `value` */

void List_unshift(List *list, void *value);    /* push new ListNode with `value to the start of List */
void *List_shift(List *list);                  /* pop last ListNode from start of List and return it's `value` */ 

void *List_remove(List *list, ListNode *node); /* removes ListNode from List and frees it */

ListNode *List_find(List *list, void *value, Fn_node_eq fn);  /* return pointer to ListNode with value `value from `list` */

List *List_copy(List *list);                   /* create new List which is copy of `list` and returns pointer to it */
List *List_join(List *list1, List *list2);     /* create new List with `list2` added to the end of `list1` and returns pointer to it  */

/*
 * split list on `node` value and returns pointer to new list with values AFTER `node`
 * 
 * list( node1("val1")->node2("val2")->node3("val3") )
 * 	=> List_split(list, "val2") results in:
 * 
 * list( node1("val2")->node2("val2") ) ++
 * 	return new_list ( node3("val3") )
 */
List *List_split(List *list, void *value, Fn_node_eq fn);  

/* Iterator for traversing the List struct
 * L - list
 * S - start point (list->first)
 * M - next Node (node->next)
 * V - node value to use in loop body
 * 
 * Example:
 * 	LIST_FOREACH(list, first, next, cur) {
 *           printf(cur->value);
 * 	}
 */
#define LIST_FOREACH(L, S, M, V) \
    ListNode *_node = NULL; \
    ListNode *V = NULL; \
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)

#endif // lcthw_List_h 
