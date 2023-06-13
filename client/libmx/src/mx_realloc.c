#include "../inc/libmx.h"

void *mx_realloc(void *ptr, size_t size) {
    void *newptr;
    size_t len;

    if (ptr == 0) return malloc(size);
    len = sizeof(ptr);
    //len = malloc_usable_size(ptr);
    if (size <= len) return ptr;
    newptr = malloc(size);
    mx_memcpy(newptr, ptr, len);
    free(ptr);
    return newptr;
}

