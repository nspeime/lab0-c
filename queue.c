#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_node =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!new_node) {
        return NULL;
    }
    INIT_LIST_HEAD(new_node);
    return new_node;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {  // queue is empty
        return false;
    }
    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element) {
        return false;
    }
    size_t len = strlen(s);
    element->value = (char *) malloc((len + 1) * sizeof(char));
    if (!element->value) {
        free(element);
        return false;
    }
    snprintf(element->value, len + 1, "%s", s);
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {  // queue is empty
        return false;
    }
    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element) {
        return false;
    }
    size_t len = strlen(s);
    element->value = (char *) malloc((len + 1) * sizeof(char));
    if (!element->value) {
        free(element);
        return false;
    }
    snprintf(element->value, len + 1, "%s", s);
    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *first_element = list_first_entry(head, element_t, list);
    list_del(&first_element->list);
    if (sp && bufsize > 0) {
        snprintf(sp, bufsize, "%s", first_element->value);
    }
    return first_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *first_element = list_last_entry(head, element_t, list);
    list_del(&first_element->list);
    if (sp && bufsize > 0) {
        snprintf(sp, bufsize, "%s", first_element->value);
    }
    return first_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    int size = 0;
    struct list_head *node;
    list_for_each (node, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return NULL;
    }
    struct list_head *slow, *fast;
    slow = fast = head->next;
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    element_t *element = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head)) {
        return false;
    }
    bool has_duplicate = false;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list) {
        bool match = false;
        if (entry->list.next != head) {
            char *s1 = entry->value;
            char *s2 = list_entry(entry->list.next, element_t, list)->value;
            match = strcmp(s1, s2) == 0;
        }
        if (match) {
            has_duplicate = match;
            list_del(&entry->list);
            q_release_element(entry);
        } else if (has_duplicate) {
            has_duplicate = match;
            list_del(&entry->list);
            q_release_element(entry);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head) {
            break;
        }
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head pending_reverse;
    INIT_LIST_HEAD(&pending_reverse);
    struct list_head *node, *safe;
    struct list_head *prev = head;
    int count = 0;
    list_for_each_safe (node, safe, head) {
        count += 1;
        if (count == k) {
            count = 0;
            list_cut_position(&pending_reverse, prev, node);
            q_reverse(&pending_reverse);
            list_splice_init(&pending_reverse, prev);
            prev = safe->prev;
        }
    }
}

/**
 * Merge two sorted linked lists into one sorted linked list, stored in `left`.
 * @param left A pointer to the head of the first list.
 * @param right A pointer to the head of the second list.
 * @param descend Indicates whether to merge lists in descending order.
 * After merging, the new list replaces the original `left` list, with `left`
 * now pointing to the head of the merged and sorted list.
 * @return The number of elements in the merged list.
 */
static int mergeTwoLists(struct list_head *left,
                         struct list_head *right,
                         bool descend)
{
    struct list_head *left_next = left->next;
    struct list_head *right_next = right->next;
    struct list_head new_head;
    INIT_LIST_HEAD(&new_head);
    int size = 0;

    if (left_next == left && right_next == right) {
        return size;
    }

    while (left_next != left && right_next != right) {
        const char *s1 = list_entry(left_next, element_t, list)->value;
        const char *s2 = list_entry(right_next, element_t, list)->value;
        if (strcmp(s1, s2) * (1 - 2 * (!descend)) >= 0) {
            struct list_head *tmp = left_next->next;
            list_move_tail(left_next, &new_head);
            left_next = tmp;
        } else {
            struct list_head *tmp = right_next->next;
            list_move_tail(right_next, &new_head);
            right_next = tmp;
        }
        size += 1;
    }
    size += q_size(left);
    list_splice_tail_init(left, &new_head);
    size += q_size(right);
    list_splice_tail_init(right, &new_head);
    list_splice_init(&new_head, left);
    return size;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (head->next == head || head->next->next == head) {
        return;
    }
    struct list_head *slow, *fast;
    slow = fast = head->next;
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head left, right;
    INIT_LIST_HEAD(&left);
    INIT_LIST_HEAD(&right);
    list_cut_position(&left, head, slow);
    list_splice_init(head, &right);
    q_sort(&left, descend);
    q_sort(&right, descend);
    mergeTwoLists(&left, &right, descend);
    list_splice_init(&left, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }
    struct list_head *node, *safe;
    char *cur_min = NULL;
    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        if (cur_min == NULL) {
            cur_min = list_entry(node, element_t, list)->value;
            continue;
        }
        element_t *e = list_entry(node, element_t, list);
        if (strcmp(e->value, cur_min) > 0) {
            list_del(node);
            q_release_element(e);
        } else {
            cur_min = e->value;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    }
    struct list_head *node, *safe;
    char *cur_max = NULL;
    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        if (cur_max == NULL) {
            cur_max = list_entry(node, element_t, list)->value;
            continue;
        }
        element_t *e = list_entry(node, element_t, list);
        if (strcmp(e->value, cur_max) < 0) {
            list_del(node);
            q_release_element(e);
        } else {
            cur_max = e->value;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    }
    int queue_size = 0;
    return queue_size;
}
