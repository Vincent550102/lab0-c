#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

void e_free(element_t *e)
{
    if (e->value)
        free(e->value);
    if (e)
        free(e);
}


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *list = malloc(sizeof(struct list_head));
    // check malloc
    if (!list)
        return NULL;
    INIT_LIST_HEAD(list);

    return list;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, head, list) {
        list_del(&pos->list);
        e_free(pos);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    // check malloc
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    // check malloc
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *first = list_first_entry(head, element_t, list);
    if (sp && first->value) {
        strncpy(sp, first->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&first->list);
    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head))
        return NULL;
    element_t *last = list_last_entry(head, element_t, list);
    if (sp && last->value) {
        strncpy(sp, last->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&last->list);
    return last;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}
/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head, *fast;
    list_for_each (fast, head) {
        if (fast != head && fast->next != head)
            fast = fast->next;
        else
            break;
        slow = slow->next;
    }
    element_t *e = list_entry(slow->next, element_t, list);
    list_del(&e->list);
    e_free(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    element_t *e, *n;
    bool flag = false;
    list_for_each_entry_safe (e, n, head, list) {
        char *value = e->value;
        char *next_value =
            e->list.next != head
                ? list_entry(e->list.next, element_t, list)->value
                : NULL;
        if (next_value && strcmp(value, next_value) == 0) {
            list_del(&e->list);
            e_free(e);
            flag = true;
        } else if (flag) {
            list_del(&e->list);
            e_free(e);
            flag = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *li;
    element_t *e1, *e2;
    list_for_each (li, head) {
        e1 = list_entry(li, element_t, list);
        if (li->next == head)
            break;
        e2 = list_entry(li->next, element_t, list);
        char *tmp = e1->value;
        e1->value = e2->value;
        e2->value = tmp;
        li = li->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *li, *tmp;
    element_t *e;
    list_for_each_safe (li, tmp, head) {
        e = list_entry(li, element_t, list);
        list_move(&e->list, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *li, *safe, *tmp = head;
    LIST_HEAD(tmp_head);
    int i = 0;
    list_for_each_safe (li, safe, head) {
        if (++i == k) {
            list_cut_position(&tmp_head, tmp, li);
            q_reverse(&tmp_head);
            list_splice_init(&tmp_head, tmp);
            tmp = safe->prev;
            i = 0;
        }
    }
}


struct list_head *merge_two_lists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; L1 && L2; *node = (*node)->next) {
        element_t *e1 = list_entry(L1, element_t, list);
        element_t *e2 = list_entry(L2, element_t, list);
        node = strcmp(e1->value, e2->value) < 0 ? &L1 : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((u_int64_t) L1 | (u_int64_t) L2);
    return head;
}

struct list_head *mergesort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast;
    list_for_each (fast, head) {
        if (fast && fast->next)
            fast = fast->next;
        else
            break;
        slow = slow->next;
    }
    struct list_head *mid = slow->next;
    mid->prev->next = NULL;


    struct list_head *left = mergesort(head), *right = mergesort(mid);
    return merge_two_lists(left, right);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = mergesort(head->next);
    struct list_head *curr = head, *next = head->next;
    while (next) {
        next->prev = curr;
        curr = next;
        next = next->next;
    }
    curr->next = head;
    head->prev = curr;

    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    struct list_head *li, *tmp;
    q_reverse(head);
    char *max_val = list_first_entry(head, element_t, list)->value;
    list_for_each_safe (li, tmp, head) {
        element_t *e = list_entry(li, element_t, list);
        if (strcmp(e->value, max_val) > 0) {
            list_del(li);
            e_free(e);
        } else
            max_val = e->value;
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_reverse(head);
    struct list_head *li, *tmp;
    char *max_val = list_first_entry(head, element_t, list)->value;
    list_for_each_safe (li, tmp, head) {
        element_t *e = list_entry(li, element_t, list);
        if (strcmp(e->value, max_val) < 0) {
            list_del(li);
            e_free(e);
        } else
            max_val = e->value;
    }
    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return 1;

    struct list_head *target = list_entry(head->next, queue_contex_t, chain)->q;
    struct list_head *li, *tmp;

    list_for_each_safe (li, tmp, head) {
        struct list_head *q = list_entry(li, queue_contex_t, chain)->q;
        if (q == target)
            continue;
        list_splice_init(q, target);
    }
    q_sort(target, descend);

    return q_size(target);
}
