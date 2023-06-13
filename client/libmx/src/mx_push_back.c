#include "../inc/libmx.h"

void mx_push_back(t_list **list, void *data) {
    t_list *new_node = (t_list*) malloc (sizeof(t_list));
    t_list *last = *list;

    new_node->data = data;
    new_node->next = NULL;

    if (*list == NULL) {
        *list = new_node;
        return;
    }

    while (last->next != NULL) last = last->next;
    last->next = new_node;
}

