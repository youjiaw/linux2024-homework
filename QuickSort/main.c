#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef struct __node {
    struct list_head list;
    long value;
} node_t;

struct list_head *list_new()
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (!new_head)
        return NULL;
    INIT_LIST_HEAD(new_head);
    return new_head;
}

int list_length(struct list_head **head)
{
    int n = 0;
    struct list_head *tmp;
    list_for_each(tmp, *head)
        ++n;
    return n;
}

void list_construct(struct list_head *head, long value)
{
    if (!head)
        return;
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node)
        return;
    INIT_LIST_HEAD(&new_node->list);
    new_node->value = value;
    list_add(&new_node->list, head);
}

void list_free(struct list_head *head)
{
    if (!head)
        return;
    node_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list)
        free(entry);
    free(head);
}

/* Verify if list is order */
static bool list_is_ordered(struct list_head *head)
{       
    bool first = true;
    long value;
    node_t *entry;
    list_for_each_entry(entry, head, list) {
        if (first) {
            value = entry->value;
            first = false;
        } else {
            if (entry->value < value)
                return false;
            value = entry->value;
        }
    }
    return true;
}

// print the list
void print_list(struct list_head *head)
{
    node_t *entry;
    list_for_each_entry(entry, head, list)
        printf("%ld ", entry->value);
    printf("\n");
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void quick_sort(struct list_head **head)
{
    int n = list_length(head);
    int value;
    int i = 0;
    int max_level = 2 * n;
    struct list_head *begin[max_level]; //list_new() will return a pointer to struct list_head, so the type should be changed
    struct list_head *result = list_new(), *left = list_new(), *right = list_new();
    
    for (int j = 1; j < max_level; j++)
        begin[j] = list_new();
    begin[0] = *head;
    
    while (i >= 0) {
        struct list_head *L = begin[i]->next, *R = begin[i]->prev;

        if (L != R) {
            struct list_head *pivot = L;
            value = list_entry(pivot, node_t, list)->value;
            list_del(pivot);
    
            node_t *entry, *safe;
            list_for_each_entry_safe(entry, safe, begin[i], list) {
                list_del(&entry->list);
                list_add(&entry->list, entry->value > value ? right : left);
            }

            list_splice_init(left, begin[i]);
            list_add(pivot, begin[i + 1]);
            list_splice_init(right, begin[i + 2]);

            i += 2;
        } else {
            if (list_is_singular(begin[i]))
                list_splice_init(begin[i], result);
            i--;
        }
    }
    *head = result;
}

int main(int argc, char **argv)
{
    struct list_head *list = list_new();

    size_t count = 100000;

    int *test_arr = malloc(sizeof(int) * count);

    if (test_arr != NULL) {
        for (int i = 0; i < count; ++i)
            test_arr[i] = i;
        shuffle(test_arr, count);

        while (count--)
            list_construct(list, test_arr[count]);

        if (list == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        
        quick_sort(&list);
        assert(list_is_ordered(list));

        list_free(list);
    }
    free(test_arr);

    return 0;
}