#include "../inc/libmx.h"

char *mx_strnew(const int size) {
    char *string = (char *)malloc(size + 1);
    if (string == NULL) return NULL;

    for (int i = 0; i < size; i++) {
        string[i] = '\0';
    }
    string[size] = '\0';
    return string;
}

