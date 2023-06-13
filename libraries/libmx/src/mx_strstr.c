#include "../inc/libmx.h"

char *mx_strstr(const char *haystack, const char *needle) {
    while (*haystack) {
        if (haystack == mx_strchr(haystack, needle[0])) {
            if (mx_strncmp(haystack, needle, mx_strlen(needle)) == 0)
                return (char*)haystack;
        }
        haystack++;
    }
    return 0;
}

