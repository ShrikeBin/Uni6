#ifndef ADA_WRAPPER_H
#define ADA_WRAPPER_H

#include <stdint.h>

/* Ada Mathlib — C-visible signatures (Ada exports with Convention C) */

uint64_t ada_gcd(uint64_t a, uint64_t b);
uint64_t ada_smallest_prime_divisor(uint64_t n);
uint64_t ada_euler_totient(uint64_t n);

typedef struct {
    int     has_solution;
    int64_t x;
    int64_t y;
} Ada_Diophantine_Solution;

Ada_Diophantine_Solution ada_solve_diophantine(int64_t a, int64_t b, int64_t c);

#endif /* ADA_WRAPPER_H */