#include "../inc/libmx.h"

void mx_printchar(const char c) {
    write(1, &c , 1);
}

