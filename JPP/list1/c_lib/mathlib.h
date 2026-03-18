#ifndef MATHLIB_H
#define MATHLIB_H

#include <stdint.h>

/* 1. Greatest Common Divisor */
uint64_t gcd(uint64_t a, uint64_t b);

/* 2. Smallest prime divisor (n > 1) */
uint64_t smallest_prime_divisor(uint64_t n);

/* 3. Euler's totient function */
uint64_t euler_totient(uint64_t n);

/* 4. Linear Diophantine equation: ax - by = c, natural solutions */
typedef struct {
    int     has_solution;  /* 1 if solution exists, 0 otherwise */
    int64_t x;
    int64_t y;
} DiophantineSolution;

DiophantineSolution solve_diophantine(int64_t a, int64_t b, int64_t c);

#endif /* MATHLIB_H */