#include "../inc/libmx.h"

void *mx_memmove(void *dst, const void *src, size_t len) {
    char *src_copy = (char *)src;
    char *dst_copy = (char *)dst;
    char *temp = malloc(len);

    for (int i = 0; i < (int)len; i++) temp[i] = src_copy[i];
    for (int i = 0; i < (int)len; i++) dst_copy[i] = temp[i];
    free(temp);
    return dst;
}

