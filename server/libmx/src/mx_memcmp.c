#include "../inc/libmx.h"

int mx_memcmp(const void *s1, const void *s2, size_t n) {
    if(!n) return 0;
    while(--n && *(char*)s1 == *(char*)s2 ) {
        s1 = (char*)s1 + 1;
        s2 = (char*)s2 + 1;
    }
    return(*((unsigned char*)s1) - *((unsigned char*)s2));
}

