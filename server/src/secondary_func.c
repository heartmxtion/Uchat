#include "../inc/server.h"

char *mx_ljoin(char *s1, char const *s2) {
    if (s1 == NULL && s2 == NULL) return NULL;
    if (s1 == NULL) return mx_strdup(s2);
    if (s2 == NULL) return mx_strdup(s1);

    char *result = mx_strnew(mx_strlen(s1) + mx_strlen(s2) + 1);
    result = mx_strcat(result, s1);
    result = mx_strcat(result, s2);

    free(s1);
    return result;
}


