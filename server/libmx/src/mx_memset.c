#include "../inc/libmx.h"

void *mx_memset(void *b, int c, size_t len) {
    char *ptr = (char*)b;

    for (size_t i = 0; i < len; i++) ptr[i] = c;
    return b;
}

