#include <assert.h>
#include <string.h>
#include "dbg.h"
#include "list.h"

int str_node_eq(void *value1, void *value2)
{
    return (strcmp((char*)value1, (char*)value2) == 0); 
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
    check(list != NULL, "Got NULL as a list value");

    LIST_FOREACH(list, first, next, cur) {
        if (cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);

error:
    return;
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
    /* TODO */
    check_mem(list1);

    List *new_list = List_create();

    /* triverse first list
     * using block here to hide declarations inside LIST_FOREACH macro
     */
    {
            LIST_FOREACH(list1, first, next, cur) {
                List_push(new_list, cur->value);
            }
    }

    /* triverse second list */
    {
            LIST_FOREACH(list2, first, next, cur) {
                List_push(new_list, cur->value);
            }
    }

    return new_list;

error:
    return NULL;
}

List *List_split(List *list, void *value, Fn_node_eq fn)
{
    check_mem(list);
    check_mem(value);

    List *second_list = List_create();
    check_mem(second_list);

    ListNode *node = NULL;

    int found = 0;

    LIST_FOREACH(list, first, next, cur) {
        if (found) {
            List_push(second_list, cur->value);
        }
        if (fn(cur->value, value)) {
            if (!found) node = cur;
            found = 1;
        }
    }
    /* if we found value to split, we will modify this node */
    if (node) {
         node->next->prev = NULL;
         node->next = NULL;
         list->count -= second_list->count;
         list->last = node;
    }

    /* if we found value to split */
    if (second_list->first) {
        return second_list;
    } else {
        List_destroy(second_list);
    }

error:
    return NULL;
}

