#include <assert.h>
#include <string.h>
#include "dbg.h"
#include "list.h"

int str_node_eq(void *value1, void *value2)
{
    return (strcmp((char*)value1, (char*)value2) == 0); 
}

int intcmp(void *value1, void *value2)
{
    return *(int *) value1 - *(int *) value2;
}

int check_consistency(List *list, char *values[], List_compare fn)
{
    int count = 0;
    {
        int i = 0;
        LIST_FOREACH(list, first, next, cur) {
            check(
                fn(cur->value, values[i]) == 0, 
                "(Forward) Wrong value in node %s != %s", 
                (char *)cur->value, values[i]
            );
            i++;
        }
        count = i;
    }

    check(list->count == count, 
          "List count is wrong! list->count: %d != count: %d", 
          list->count, count);
    
    {
        int i = count - 1;
        LIST_FOREACH(list, last, prev, cur) {
            check(
                fn(cur->value, values[i]) == 0, 
                "(Backward) Wrong value in node %s != %s", 
                (char *)cur->value, values[i]
            );
            i--;
        }
    }

    return 0;
error:
    return -1;
}

void List_print_str(List *list)
{
    int i = 0;
    ListNode *node = list->first;
    while (node) {
        printf("%s <=> ", (char*) node->value);
        node = node->next;
        i = 1;
    }
    if (i == 0)
        printf("EMPTY\n");
    else
        printf("END\n");
}

void ListNode_print_str(ListNode *node)
{
    int i = 0;
    while (node) {
        printf("%s <=> ", (char*) node->value);
        node = node->next;
        i = 1;
    }
    if (i == 0)
        printf("EMPTY\n");
    else
        printf("END\n");
}

List *List_create(void)
{
    List* res = NULL;
    res = calloc(1, sizeof(List));
    check_mem(res);

    return (List*) res;

error:
    exit(EXIT_FAILURE);
}

void List_destroy(List *list)
{
    if (list == NULL) return;

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);
    list = NULL;
}

void List_clear(List *list)
{
    check(list != NULL, "Got NULL as a list value");

    LIST_FOREACH(list, first, next, cur) {
        free(cur->value);
    }

error:
    return;
}

void List_clear_destroy(List *list)
{
    check(list != NULL, "Got NULL as a list value");

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev->value);
            free(cur->prev);
        }
    }
    free(list->last);
    free(list);

error:
    return;
}

void List_push(List *list, void *value)
{
    check(list != NULL, "Got NULL as a list value, doing nothing");

    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }

    list->count++;
    assert(list->first != NULL && "List empty after push\n");
    assert(list->last  != NULL && "List empty after push\n");

error:
    return;
}

void *List_pop(List *list)
{
    check(list != NULL, "Got NULL as a list value, doing nothing");
    ListNode *node = list->last;
    void *res = (node != NULL ? List_remove(list, node) : NULL);
    assert(list->count >= 0 && "List count less than 0\n");
    return res;

error:
    return NULL;
}

void List_unshift(List *list, void *value)
{
    check(list != NULL, "Got NULL as a list value, doing nothing");
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;
    assert(list->first != NULL && "List is empty after unshift\n");
    assert(list->last != NULL && "List is empty after unshift\n");

error:
    return;
}

void *List_shift(List *list)
{
    check(list != NULL, "Got NULL as a list value, doing nothing");
    ListNode *node = list->first;

    void *res = (node != NULL ? List_remove(list, node) : NULL); 
    assert(list->count >= 0 && "List count less than 0 after List_shift\n");
    return res;

error:
    return NULL;
}

void *List_remove(List *list, ListNode *node)
{
    void *result = NULL;
    check(list != NULL, "Got NULL as a list value, doing nothing");

    check(list->first && list->last, "List is empty");
    check(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if (node == list->first) {
        list->first = node->next;
        check(list->first != NULL,
              "Invalid list, somehow got a first that is NULL.");
        list->first->next = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL,
              "Invalid list, somehow got a next that is NULL.");
        list->last->next = NULL;
    } else {
        ListNode *after = node->next;
        ListNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);
    assert(list->count >= 0 && "List count less than 0 after List_remove\n");

error:
    return result;
}

ListNode *List_find(List *list, void* value, Fn_node_eq fn)
{
    check_mem(list);

    LIST_FOREACH(list, first, next, cur) {
        if (fn(cur->value, value)) {
            return cur;
        }
    }

error:
    return NULL;
}

List *List_copy(List *list)
{
    /* TODO */
    check_mem(list);

    List *new_list = List_create();
    LIST_FOREACH(list, first, next, cur) {
        List_push(new_list, cur->value);
    }
    return new_list;

error:
    return NULL;
}

List *List_join(List *list1, List *list2)
{
    check_mem(list1);
    check_mem(list2);

    /* Both empty or second empty */
    if (list2->first == NULL) {
        free(list2);
        list2 = NULL;
        return list1;
    }
    
    /* First empty */
    if (list1->first == NULL) {
        free(list1);
        list1 = NULL;
        return list2;
    }
    
    list1->last->next = list2->first;
    list2->first->prev = list1->last;
    list1->count += list2->count;
    list1->last = list2->last;
    
    free(list2);
    list2 = NULL;

    return list1;

error:
    return NULL;
}

List *List_split(List *list)
{
    check_mem(list);
    List *second = List_create();

    if (list->count == 0 || list->count == 1) return second;

    int i = 0;
    int count = list->count - (list->count / 2);
    ListNode *node = NULL;

    for (node = list->first, i = 0; node != NULL; node = node->next, i++) {
        if (i == count - 1) {     /* we are at last node of `first` */
            second->first       = node->next;
            second->first->prev = NULL;
            second->last        = list->last;
            second->count       = list->count / 2;
            list->last          = node;
            list->last->next    = NULL;
            list->count         = count;
            break;
        }
    }

    return second;
error:
    return NULL;
}

void List_append_node(List *list, ListNode *node)
{
    /* FIX: this is wrong should be error */
    if (list == NULL) return;

    if (list->first == NULL) {
        list->first       = node;
        list->last        = node;
        list->first->next = NULL;
        list->first->prev = NULL;
        list->count       = 1;
        return;
    }

    list->last->next = node;
    node->prev = list->last;
    node->next = NULL;
    list->last = node;
    list->count++;
}

ListNode *List_shift_get(List *list)
{
    check_mem(list);
    ListNode *node = list->first;
    
    /* Empty list */
    if (node == NULL) return NULL;

    ListNode *node_next = node->next;
    if (node_next) {
        list->first = node_next;
        list->first->prev = NULL;
        list->count--;
    } else { /* single node list */
        list->first = NULL;
        list->last = NULL;
        list->count = 0;
    }
    node->next = NULL;
    node->prev = NULL;
    
    return node;
error:
    return NULL;
}

void List_insert_sorted(List *list, void *value, List_compare fn)
{
    check_mem(list);    
    
    int cmp = 0;
    ListNode *insert = (ListNode*) calloc(1, sizeof(ListNode));
    insert->value = value;

    /* insert into empty list */
    if (!list->first) {
        list->first       = insert;
        list->first->next = NULL;
        list->last        = insert;
        list->last->prev  = NULL;
        list->count++;
        return;
    }

    ListNode *node      = list->first;
    ListNode *node_next = list->first->next;

    /* head insert */
    cmp = fn(node->value, value);
    if (cmp > 0) {
        node->prev        = insert;
        insert->next      = node;
        list->first       = insert;
        list->first->prev = NULL;
        list->count++;
        return;
    }
    
    /* insert between nodes */
    while (node_next != NULL) {
        if (fn(node->value, value) <= 0 && fn(value, node_next->value) < 0) {
            node->next      = insert;
            node_next->prev = insert;
            insert->next    = node_next;
            insert->prev    = node;
            list->count++;
            return;
        }
        node      = node_next;
        node_next = node_next->next;
    }

    /* tail insert */
    cmp = fn(node->value, value);
    if (cmp <= 0) {
        insert->prev = node;
        insert->next = NULL;
        node->next = insert;
        list->last = insert;
        list->count++;
        return;
    }
    
    /* fallthrough */
error:
    return;
}

List *List_join_sorted(List *list1, List *list2, List_compare fn)
{
    /* second is empty */
    int left_count  = list1->count;
    int right_count = list2->count;
    ListNode *node  = NULL;

    /* pushing to list 1 */
    List *list = list1;

    while (left_count > 0 && right_count > 0) {
        int cmp = fn(list1->first->value, list2->first->value);
        if (cmp > 0) {
            node = List_shift_get(list2);
            List_append_node(list, node);
            right_count--;
        } else {
            node = List_shift_get(list1);
            List_append_node(list, node);
            left_count--;
        }
    }
    while (left_count > 0) {
        node = List_shift_get(list1);
        List_append_node(list, node);
        left_count--;
    }
    while (right_count > 0) {
        node = List_shift_get(list2);
        List_append_node(list, node);
        right_count--;
    }
    List_destroy(list2);
    return list;
}

