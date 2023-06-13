#include "../inc/libmx.h"

unsigned long mx_hex_to_nbr(const char *hex) {
    if (hex == NULL) return 0;
    unsigned long res = 0;
    short int num = 0;

    for (int i = 0; hex[i] != '\0'; i++) {
        res *= 16;
        if (48 <= hex[i] && hex[i] <= 57) num = hex[i] - 48;
        if ((65 <= hex[i] && hex[i] <= 90) || (97 <= hex[i] && hex[i] <= 122)) {
            if ('A' <= hex[i] && hex[i] <= 'F') 
                num = hex[i] - 55;
            else if ('a' <= hex[i] && hex[i] <= 'f') 
                num = hex[i] - 87;
            else return 0;
        }
        res += num;
    }
    return res;
}

