#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdint.h>

uint64_t gcd(uint64_t a, uint64_t b);

uint64_t smallest_prime_divisor(uint64_t n);

uint64_t euler_totient(uint64_t n);

typedef struct {
    int     has_solution;  // 1 if solution exists, 0 other
    int64_t x;
    int64_t y;
} DiophantineSolution;

DiophantineSolution solve_diophantine(int64_t a, int64_t b, int64_t c);

#endif /* MATHLIB_H */