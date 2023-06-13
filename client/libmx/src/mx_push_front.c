#include "../inc/libmx.h"

void mx_push_front(t_list **list, void *data) {
    t_list *new_node = (t_list*) malloc(sizeof(t_list));
    new_node->data = data;
    new_node->next = *list;
    *list = new_node;
}

