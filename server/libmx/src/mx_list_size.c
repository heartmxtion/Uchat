#include "../inc/libmx.h"

int mx_list_size(t_list *list) {
    int size = 0;

    while (list) {
        list = list->next;
        size++;
    }
    return size;
}

