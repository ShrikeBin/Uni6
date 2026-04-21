#ifndef UTILS_HPP
#define UTILS_HPP

#include <random>

template<typename T>
T fast_pow(T base, unsigned long long exp, T identity) {
    T result = identity;
    while (exp > 0) {
        if (exp & 1) result = result * base;
        base = base * base;
        exp >>= 1;
    }
    return result;
}

static long long ext_gcd(long long a, long long b, long long &x) {
    long long old_x = 1, xx = 0, old_r = a, r = b;
    while (r != 0) {
        long long q = old_r / r;
        long long tmp = r;   
        r   = old_r - q * r;   
        old_r = tmp;
        tmp = xx;  
        xx  = old_x - q * xx;  
        old_x = tmp;
    }
    x = old_x;
    return old_r;
}

#endif