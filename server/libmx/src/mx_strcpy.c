#include "../inc/libmx.h"

char *mx_strcpy(char *dst, const char *src) {
    for (int i = 0; src[i] != '\0'; i++) {
        dst[i] = src[i];
        dst[i + 1] = '\0';
    }
    return dst;
}

