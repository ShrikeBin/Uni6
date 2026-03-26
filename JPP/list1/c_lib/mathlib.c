#include "mathlib.h"
#include <stdint.h>

uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

uint64_t smallest_prime_divisor(uint64_t n) {
    if (n % 2 == 0) return 2;
    for (uint64_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return i;
    }
    return n;
}

uint64_t euler_totient(uint64_t n) {
    uint64_t result = n;
    for (uint64_t p = 2; p * p <= n; p++) {
        if (n % p == 0) {
            while (n % p == 0) n /= p;
            result -= result / p;
        }
    }
    if (n > 1) result -= result / n;
    return result;
}

static int64_t extended_gcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    int64_t x1, y1;
    int64_t g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

DiophantineSolution solve_diophantine(int64_t a, int64_t b, int64_t c) {
    DiophantineSolution sol = {0, 0, 0};
    if (a <= 0 || b <= 0) return sol;

    int64_t x0, y0;
    int64_t g = extended_gcd(a, b, &x0, &y0);

    if (c % g != 0) return sol;

    int64_t scale = c / g;
    x0 *= scale;
    y0 *= scale;

    int64_t step_x = b / g;
    int64_t step_y = a / g;

    int64_t t_min_x, t_min_y, t;

    int64_t num_x = 1 - x0;

    t_min_x = num_x / step_x;

    if (num_x % step_x != 0 && (num_x ^ step_x) < 0) {
        t_min_x--;
    } else if (num_x % step_x != 0) {
        t_min_x++;
    }

    int64_t num_y = 1 + y0;
    
    t_min_y = num_y / step_y;

    if (num_y % step_y != 0 && (num_y ^ step_y) < 0){ 
        t_min_y--;
    } else if (num_y % step_y != 0) {
        t_min_y++;
    }

    t = t_min_x > t_min_y ? t_min_x : t_min_y;

    int64_t x_sol = x0 + step_x * t;
    int64_t y_sol = -y0 + step_y * t;

    if (x_sol > 0 && y_sol > 0) {
        sol.has_solution = 1;
        sol.x = x_sol;
        sol.y = y_sol;
    }
    return sol;
}