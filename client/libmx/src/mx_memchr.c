#include "../inc/libmx.h"

void *mx_memchr(const void *s, int c, size_t n) {
    unsigned char *ptr = (unsigned char *)s;

    for (; n > 0; n--) {
        if (*ptr == (unsigned char)c) return ptr;
        ptr++;
    }
    return NULL;
}

