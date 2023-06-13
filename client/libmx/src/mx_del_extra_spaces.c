#include "../inc/libmx.h"

char *mx_del_extra_spaces(const char *str) {
    if (!str) return NULL;
    int i = 0;
    int j = 0;
    char *string = mx_strnew(mx_strlen(str));

    while (str[i]) {
        if (!(mx_isspace(str[i]))) {
            string[j] = str[i];
            j++;
        }
        if (mx_isspace(str[i + 1]) 
            && !(mx_isspace(str[i]))) {
            string[j] = ' ';
            j++;
        }
        i++;
    }
    char *result = mx_strtrim(string);
    mx_strdel(&string);
    
    return result;
}

