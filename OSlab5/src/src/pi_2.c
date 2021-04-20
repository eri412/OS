#include "pi.h"

float pi(int n) {
    if (n < 1) {
        return 0;
    }
    float res = 1;
    for (int i = 1; i <= n; ++i) {
        float a = 2 * ((i + 1)/2);
        float b = 1 + 2 * (i/2);
        res *= a / b;
    }
    return res * 2;
}
