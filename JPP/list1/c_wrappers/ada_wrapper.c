#include "ada_wrapper.h"

// Remember to compile ada with C convention (See .ads file for details)
extern uint64_t mathlib__gcd(uint64_t a, uint64_t b);
extern uint64_t mathlib__smallest_prime_divisor(uint64_t n);
extern uint64_t mathlib__euler_totient(uint64_t n);

typedef struct {
    int     has_solution;
    int64_t x;
    int64_t y;
} Raw_Ada_Sol;

extern Raw_Ada_Sol mathlib__solve_diophantine(int64_t a, int64_t b, int64_t c);

uint64_t ada_gcd(uint64_t a, uint64_t b) {
    return mathlib__gcd(a, b);
}
uint64_t ada_smallest_prime_divisor(uint64_t n) {
    return mathlib__smallest_prime_divisor(n);
}
uint64_t ada_euler_totient(uint64_t n) {
    return mathlib__euler_totient(n);
}
Ada_Diophantine_Solution ada_solve_diophantine(int64_t a, int64_t b, int64_t c) {
    Raw_Ada_Sol r = mathlib__solve_diophantine(a, b, c);
    Ada_Diophantine_Solution s;
    s.has_solution = r.has_solution;
    s.x = r.x;
    s.y = r.y;
    return s;
}