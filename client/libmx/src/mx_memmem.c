#include "../inc/libmx.h"

void *mx_memmem(const void *big, size_t big_len, const void *little, size_t little_len) {
    char *big_copy = NULL;
	char *little_copy = NULL;

	if (big_len > 0 && little_len > 0 && big_len >= little_len) {
		big_copy = (char*)big;
		little_copy = (char*)little;
		while (*big_copy) {
			if (mx_memcmp(big_copy, little_copy, little_len - 1) == 0) return big_copy;
			big_copy++;
		}
	}
    return NULL;
}

