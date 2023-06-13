#include "../inc/libmx.h"

int mx_quicksort(char **arr, int left, int right) {
    if (!arr) return -1;
    static int swaps = 0;
    
    if (left < right) {
        char *pivot = arr[right];
        char *temp;
        
        int i = (left - 1);

        for (int j = left; j < right; j++) {
            if (mx_strlen(arr[j]) < mx_strlen(pivot)) {
                i++;

                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        temp = arr[i + 1];
        arr[i + 1] = arr[right];
        arr[right] = temp;
        
        mx_quicksort(arr, left, i);
        mx_quicksort(arr, i + 2, right);
        swaps++;
    }
    return swaps - 1;
}

