#include "../inc/libmx.h"

bool mx_isdigit(int c) {
    if (c >= '\060' && c <= '\071') {
        return 1;
    }
    else {
        return 0;
    }
}

