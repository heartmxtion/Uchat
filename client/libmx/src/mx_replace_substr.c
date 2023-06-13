#include "../inc/libmx.h"

char *mx_replace_substr(const char *str, const char *sub, const char *replace) {
    if (!sub || !str || !replace) return NULL;
    int size = mx_strlen(str) - (mx_count_substr(str, sub) 
                *mx_strlen(sub)) + (mx_count_substr(str, sub) * mx_strlen(replace));
    char *result = mx_strnew(size);
    char *str_copy = mx_strnew(mx_strlen(str));

    mx_strcpy(str_copy, str);
    for (int i = 0; i < size; i++) {
        int index = mx_get_substr_index(str_copy, sub);
        if (i != index) result[i] = str[i];
        else if (i == index) {
            for (int j = 0; j < mx_strlen(replace); j++) {
                str_copy[j + index] = replace[j];
                result[j + index] = replace[j];
            }
            i += mx_strlen(replace) - 1; 
        }
    }
    return result;
}

