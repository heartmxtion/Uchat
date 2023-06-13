#include "../inc/libmx.h"

void mx_print_strarr(char **arr, const char *delim) {
    if (!arr || !delim) return;

    for (int i = 0; arr[i] != NULL; i++) {
        for (int j = 0; j < mx_strlen(arr[i]); j++) {
            write(1, &arr[i][j], 1);
        }
        if (arr[i + 1] != NULL) write(1, delim, mx_strlen(delim));
    }
    write(1, "\n", 1);
}

