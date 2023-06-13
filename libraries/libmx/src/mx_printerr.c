#include "../inc/libmx.h"


// add in libmx
void mx_printerr(const char *s) {
    write(2, s, mx_strlen(s));
}

void mx_printerr_char(const char *s) {
    write(2, s, 1);
}

