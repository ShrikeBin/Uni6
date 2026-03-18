//! Rust test program calling C, Ada, and Rust libraries.

use mathlib as rust_lib;

/* ---- C library FFI ---- */
#[repr(C)]
struct CDiophSol { has_solution: i32, x: i64, y: i64 }

extern "C" {
    fn gcd(a: u64, b: u64) -> u64;
    fn smallest_prime_divisor(n: u64) -> u64;
    fn euler_totient(n: u64) -> u64;
    fn solve_diophantine(a: i64, b: i64, c: i64) -> CDiophSol;
}

/* ---- Ada library FFI ---- */
#[repr(C)]
struct AdaDiophSol { has_solution: i32, x: i64, y: i64 }

extern "C" {
    #[link_name = "mathlib__gcd"]
    fn ada_gcd(a: u64, b: u64) -> u64;
    #[link_name = "mathlib__smallest_prime_divisor"]
    fn ada_smallest_prime_divisor(n: u64) -> u64;
    #[link_name = "mathlib__euler_totient"]
    fn ada_euler_totient(n: u64) -> u64;
    #[link_name = "mathlib__solve_diophantine"]
    fn ada_solve_diophantine(a: i64, b: i64, c: i64) -> AdaDiophSol;
}

fn print_dioph(lib: &str, s: (bool, i64, i64)) {
    if s.0 {
        println!("  [{lib}] x={}, y={}", s.1, s.2);
    } else {
        println!("  [{lib}] No natural solution");
    }
}

fn main() {
    println!("=== GCD(48, 18) ===");
    let (c_g, ada_g, rust_g) = unsafe {
        (gcd(48, 18), ada_gcd(48, 18), rust_lib::gcd(48, 18))
    };
    println!("  C: {c_g}  Ada: {ada_g}  Rust: {rust_g}");

    println!("\n=== Smallest prime divisor of 91 ===");
    let (c_s, ada_s, rust_s) = unsafe {
        (smallest_prime_divisor(91),
         ada_smallest_prime_divisor(91),
         rust_lib::smallest_prime_divisor(91))
    };
    println!("  C: {c_s}  Ada: {ada_s}  Rust: {rust_s}");

    println!("\n=== Euler totient of 36 ===");
    let (c_e, ada_e, rust_e) = unsafe {
        (euler_totient(36), ada_euler_totient(36), rust_lib::euler_totient(36))
    };
    println!("  C: {c_e}  Ada: {ada_e}  Rust: {rust_e}");

    println!("\n=== Diophantine: 5x - 3y = 1 ===");
    unsafe {
        let cs = solve_diophantine(5, 3, 1);
        let as_ = ada_solve_diophantine(5, 3, 1);
        let rs = rust_lib::solve_diophantine(5, 3, 1);
        print_dioph("C",    (cs.has_solution != 0, cs.x, cs.y));
        print_dioph("Ada",  (as_.has_solution != 0, as_.x, as_.y));
        print_dioph("Rust", (rs.has_solution, rs.x, rs.y));
    }
}