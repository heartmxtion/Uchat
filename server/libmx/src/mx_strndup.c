#include "../inc/libmx.h"

char *mx_strndup(const char *s1, size_t n) {
    char *copy = mx_strnew(n);

    for (int i = 0; i < (int)n; i++) copy[i] = s1[i];
    copy[n] = '\0';
    return copy;
}

