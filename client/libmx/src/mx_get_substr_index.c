#include "../inc/libmx.h" 

int mx_get_substr_index(const char *str, const char *sub) {
    if (!str || !sub) return -2;

    for (int i = 0; i < mx_strlen(str); i++) {
        int count = 0;
        
        for (int j = 0; j < mx_strlen(sub); j++) {
            if (str[i + j] == sub[j]) count++;
        }
        if (count == mx_strlen(sub)) return i;
    }
    return -1;
}

