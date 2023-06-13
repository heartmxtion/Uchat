#include "../inc/libmx.h"

int mx_count_words(const char *str, char delimiter) {
    int count = 0;
    int sequence = 1;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == delimiter) {
            sequence = 1;
        }
        else if (sequence) {
            count++;
            sequence = 0;
        }
    }
    return count;
}

