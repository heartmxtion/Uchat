#include "../inc/libmx.h"

void mx_printint(int n) {
    int number[256];
    int count = 0;

    if (n == -2147483648) {
        write(1, "-2147483648", 11);
        return;
    }

    if (n < 0) {
        n *= -1;
        mx_printchar('-');
    }
    if (n == 0) mx_printchar('0');
    while (n != 0) {
        number[count] = n % 10;
        n /= 10;
        count++;
    }
    for (int i = count - 1; i >= 0; i--) {
        mx_printchar(number[i] + 48);
    }
}

