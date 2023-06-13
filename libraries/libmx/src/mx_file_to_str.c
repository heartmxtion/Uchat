#include "../inc/libmx.h"

char *mx_file_to_str(const char *filename) {
    if (!filename) return NULL;
    int length = 0;
    char c;
    int file = open(filename, O_RDONLY);

    if (file == -1) return NULL;
    while (read(file, &c, 1) != 0) length++;
    close(file);
    char *text = mx_strnew(length);

    file = open(filename, O_RDONLY);
    read(file, text, length);
    text[length] = '\0';
    close(file);
    return text;
}

