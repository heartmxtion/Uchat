#include "../inc/libmx.h"

char *mx_itoa(int number) {
    char buffer[12];
    int len = 0;
    int digit;
    int power = 1;

    for (int i = number / 10; i != 0; i /= 10) {
        power *= 10;
    }

    if (number < 0) {
        buffer[len++] = '-';
    }
    while (power != 0) {
        digit = number / power % 10;
        if (number < 0) {
            digit *= -1;
        }
        buffer[len++] = (char)('0' + digit);
        power /= 10;
    }
    buffer[len++] = '\0';
    char *res = malloc(sizeof(char) * len);
    for(int i = 0; i < len; i++) res[i] = buffer[i];
    return res;
}

