#include "../inc/libmx.h"

void mx_str_reverse(char *s) {
    int len = mx_strlen(s) - 1;
    char temp;

    for (int i = 0; i <= len; i++) {
        temp = s[i];
        s[i] = s[len];
        s[len--] = temp;
    }
}

