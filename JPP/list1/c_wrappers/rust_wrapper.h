#ifndef RUST_WRAPPER_H
#define RUST_WRAPPER_H

#include <stdint.h>

uint64_t rust_gcd(uint64_t a, uint64_t b);
uint64_t rust_smallest_prime_divisor(uint64_t n);
uint64_t rust_euler_totient(uint64_t n);

typedef struct {
    int     has_solution;
    int64_t x;
    int64_t y;
} Rust_Diophantine_Solution;

Rust_Diophantine_Solution rust_solve_diophantine(int64_t a, int64_t b, int64_t c);

#endif /* RUST_WRAPPER_H */