#include "../inc/libmx.h"

int mx_atoi(const char *str) {
    int first = -1;
    int last = -1;
    bool minus = false;
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '-' && mx_isdigit(str[i + 1]) == true) {
            minus = true;
            continue;
        }
        if (mx_isspace(str[i]) == false && first == -1) {
            first = i;
        }
        if (mx_isdigit(str[i]) == false) {
            return 0;
        }
    }
    if (last == -1) {
        for (int i = first; str[i] != '\0'; i++) {
            result = result * 10 + (int)str[i] - 48;
        }
    }
    else {
        for (int i = first; str[i] != last; i++) {
            result = result * 10 + (int)str[i] - 48;
        }
    }
    return minus == false ? result : result * -1;
}
