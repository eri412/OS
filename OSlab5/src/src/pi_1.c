#include "pi.h"

float pi(int n) {
    if (n < 1) {
        return 0;
    }
    float res = 1;
    int sign = -1;
    for (int i = 1; i < n; ++i) {
        res += sign * ((float) 1 / (1 + i*2));
        sign *= -1;
    }
    return res * 4;
}
