#include "../inc/libmx.h"

char *mx_nbr_to_hex(unsigned long nbr) {
     if (sizeof(nbr) > sizeof(unsigned long)) return 0;
    char arr[50];
    int g = 0;

    for (unsigned long i = nbr; i > 0; i /= 16) {
        int num = i % 16;

        if (num > 9) arr[g++] = (char)(num + 87);
        else arr[g++] = (char)(num + 48);
    }

    char *res = malloc(g + 1);

    for (int i = 0; i < g; i++) {
        res[i] = arr[g - i - 1];
        res[i + 1] = '\0';
    }
    return res;
}

