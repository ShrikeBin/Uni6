"""
Python test program calling C, Ada and Rust shared libraries via ctypes.
"""
import ctypes
import os
import sys

# ── Locate libraries ──────────────────────────────────────────────────────────
BUILD = os.path.join(os.path.dirname(__file__), "..")

def load(name: str):
    paths = [
        os.path.join(BUILD, name),
        os.path.join(BUILD, "build", name),
    ]
    for p in paths:
        p = os.path.abspath(p)
        if os.path.exists(p):
            return ctypes.CDLL(p)
    raise FileNotFoundError(f"Cannot find shared library: {name}")

libc_math  = load("c_lib/libmathlib_c.so")
libada     = load("ada_lib/libmathlib_ada.so")
librust    = load("rust_lib/target/release/libmathlib.so")

# ── Diophantine result structure (same layout for all three libs) ─────────────
class DiophSol(ctypes.Structure):
    _fields_ = [
        ("has_solution", ctypes.c_int),
        ("x",            ctypes.c_int64),
        ("y",            ctypes.c_int64),
    ]

# ── Bind C library ────────────────────────────────────────────────────────────
def bind_c(lib, prefix=""):
    lib[f"{prefix}gcd"]                   = (ctypes.c_uint64, [ctypes.c_uint64, ctypes.c_uint64])
    lib[f"{prefix}smallest_prime_divisor"]= (ctypes.c_uint64, [ctypes.c_uint64])
    lib[f"{prefix}euler_totient"]         = (ctypes.c_uint64, [ctypes.c_uint64])
    lib[f"{prefix}solve_diophantine"]     = (DiophSol,        [ctypes.c_int64]*3)

def setup(lib, syms):
    for name, (restype, argtypes) in syms.items():
        fn = getattr(lib, name)
        fn.restype  = restype
        fn.argtypes = argtypes

c_syms = {
    "gcd":                    (ctypes.c_uint64, [ctypes.c_uint64, ctypes.c_uint64]),
    "smallest_prime_divisor": (ctypes.c_uint64, [ctypes.c_uint64]),
    "euler_totient":          (ctypes.c_uint64, [ctypes.c_uint64]),
    "solve_diophantine":      (DiophSol,        [ctypes.c_int64, ctypes.c_int64, ctypes.c_int64]),
}
setup(libc_math, c_syms)

ada_syms = {
    "mathlib__gcd":                    (ctypes.c_uint64, [ctypes.c_uint64, ctypes.c_uint64]),
    "mathlib__smallest_prime_divisor": (ctypes.c_uint64, [ctypes.c_uint64]),
    "mathlib__euler_totient":          (ctypes.c_uint64, [ctypes.c_uint64]),
    "mathlib__solve_diophantine":      (DiophSol,        [ctypes.c_int64, ctypes.c_int64, ctypes.c_int64]),
}
setup(libada, ada_syms)

rust_syms = {
    "rust_gcd":                    (ctypes.c_uint64, [ctypes.c_uint64, ctypes.c_uint64]),
    "rust_smallest_prime_divisor": (ctypes.c_uint64, [ctypes.c_uint64]),
    "rust_euler_totient":          (ctypes.c_uint64, [ctypes.c_uint64]),
    "rust_solve_diophantine":      (DiophSol,        [ctypes.c_int64, ctypes.c_int64, ctypes.c_int64]),
}
setup(librust, rust_syms)

# ── Helper ────────────────────────────────────────────────────────────────────
def fmt_dioph(label: str, sol: DiophSol) -> str:
    if sol.has_solution:
        return f"  [{label}] x={sol.x}, y={sol.y}"
    return f"  [{label}] No natural solution"

# ── Tests ─────────────────────────────────────────────────────────────────────
print("=== GCD(48, 18) ===")
print(f"  C:    {libc_math.gcd(48, 18)}")
print(f"  Ada:  {libada.mathlib__gcd(48, 18)}")
print(f"  Rust: {librust.rust_gcd(48, 18)}")

print("\n=== Smallest prime divisor of 91 ===")
print(f"  C:    {libc_math.smallest_prime_divisor(91)}")
print(f"  Ada:  {libada.mathlib__smallest_prime_divisor(91)}")
print(f"  Rust: {librust.rust_smallest_prime_divisor(91)}")

print("\n=== Euler totient of 36 ===")
print(f"  C:    {libc_math.euler_totient(36)}")
print(f"  Ada:  {libada.mathlib__euler_totient(36)}")
print(f"  Rust: {librust.rust_euler_totient(36)}")

print("\n=== Diophantine: 5x - 3y = 1 ===")
print(fmt_dioph("C",    libc_math.solve_diophantine(5, 3, 1)))
print(fmt_dioph("Ada",  libada.mathlib__solve_diophantine(5, 3, 1)))
print(fmt_dioph("Rust", librust.rust_solve_diophantine(5, 3, 1)))