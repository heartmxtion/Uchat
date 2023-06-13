#include "../inc/libmx.h"

int mx_binary_search(char **arr, int size, const char *s, int *count) {
    int mid = (size - 1) / 2;
    int first = 0;
    int last = size - 1;
    
    while (first <= last) {
        (*count)++;
        if(mx_strcmp(arr[mid], s) < 0) first = mid + 1;
        else if (mx_strcmp (arr[mid], s) == 0) return mid;
        else last = mid - 1;

        mid = (first + last) / 2;
    }
    if (first > last) {
        *count = 0;
        return -1;
    }
    return -1;
}

