#include <stdio.h>
#include <stdint.h>
#include "../c_lib/mathlib.h"
#include "../c_wrappers/ada_wrapper.h"
#include "../c_wrappers/rust_wrapper.h"

static void print_dioph(const char *lib, int has, int64_t x, int64_t y) {
    if (has)
        printf("  [%s] x=%lld, y=%lld\n", lib, (long long)x, (long long)y);
    else
        printf("  [%s] No natural solution\n", lib);
}

int main(void) {
    printf("=== GCD(48, 18) ===\n");
    printf("  C:    %llu\n", (unsigned long long)gcd(48, 18));
    printf("  Ada:  %llu\n", (unsigned long long)ada_gcd(48, 18));
    printf("  Rust: %llu\n", (unsigned long long)rust_gcd(48, 18));

    printf("\n=== Smallest prime divisor of 91 ===\n");
    printf("  C:    %llu\n", (unsigned long long)smallest_prime_divisor(91));
    printf("  Ada:  %llu\n", (unsigned long long)ada_smallest_prime_divisor(91));
    printf("  Rust: %llu\n", (unsigned long long)rust_smallest_prime_divisor(91));

    printf("\n=== Euler totient of 36 ===\n");
    printf("  C:    %llu\n", (unsigned long long)euler_totient(36));
    printf("  Ada:  %llu\n", (unsigned long long)ada_euler_totient(36));
    printf("  Rust: %llu\n", (unsigned long long)rust_euler_totient(36));

    printf("\n=== Diophantine: 5x - 3y = 1 ===\n");
    {
        DiophantineSolution cs = solve_diophantine(5, 3, 1);
        print_dioph("C",    cs.has_solution, cs.x, cs.y);
    }
    {
        Ada_Diophantine_Solution as = ada_solve_diophantine(5, 3, 1);
        print_dioph("Ada",  as.has_solution, as.x, as.y);
    }
    {
        Rust_Diophantine_Solution rs = rust_solve_diophantine(5, 3, 1);
        print_dioph("Rust", rs.has_solution, rs.x, rs.y);
    }

    return 0;
}