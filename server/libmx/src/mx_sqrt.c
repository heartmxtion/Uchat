#include "../inc/libmx.h"

int mx_sqrt(int x) {
    if (x < 0) return 0;
    double sqrt;
    double temp = 0;

    sqrt = x / 2;
    while(sqrt != temp) {
        temp = sqrt;
        sqrt = ( x / temp + temp) / 2;
    }
    
    if (sqrt - (int)sqrt == 0) return sqrt;
    return 0;
}

