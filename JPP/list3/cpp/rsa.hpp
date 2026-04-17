#ifndef RSA_HPP
#define RSA_HPP

#include <stdexcept>
#include <random>
#include <cmath>

// Fast modular exponentiation: base^exp using O(log exp) multiplications
// Works on any type T that supports operator* and has a multiplicative identity
template<typename T>
T fast_pow(T base, unsigned long exp, T identity) {
    T result = identity;
    while (exp > 0) {
        if (exp & 1) result = result * base;
        base = base * base;
        exp >>= 1;
    }
    return result;
}

// Extended GCD — returns gcd(a, b) and sets x such that a*x + b*y = gcd
static long long ext_gcd(long long a, long long b, long long &x) {
    long long old_x = 1, xx = 0, old_r = a, r = b;
    while (r != 0) {
        long long q = old_r / r;
        long long tmp = r;   r   = old_r - q * r;   old_r = tmp;
                  tmp = xx;  xx  = old_x - q * xx;  old_x = tmp;
    }
    x = old_x;
    return old_r;
}

// Euler's totient for n = p * q (both prime)
static unsigned long euler_totient(unsigned p, unsigned q) {
    return (unsigned long)(p - 1) * (q - 1);
}

// Choose a random e in [2, phi-1] with gcd(e, phi) == 1
static unsigned long choose_e(unsigned long phi, std::mt19937 &rng) {
    std::uniform_int_distribution<unsigned long> dist(2, phi - 1);
    unsigned long e;
    long long dummy;
    do {
        e = dist(rng);
    } while (ext_gcd((long long)e, (long long)phi, dummy) != 1);
    return e;
}

// Compute modular inverse of e mod phi
static unsigned long mod_inverse(unsigned long e, unsigned long phi) {
    long long x;
    long long g = ext_gcd((long long)e, (long long)phi, x);
    if (g != 1) throw std::domain_error("RSA: e has no inverse mod phi");
    return (unsigned long)(((x % (long long)phi) + (long long)phi) % (long long)phi);
}

// ─── RSA class template ──────────────────────────────────────────────────────
// T must be constructible from int and support operator* and Ring-like semantics.
// The modulus n = p*q is baked into T via the template parameter (Ring<n>),
// so encrypt/decrypt just do fast_pow inside that ring.

template<typename T>
class RSA {
    unsigned  n_;           // p * q
    T         public_key_;  // e
    T         private_key_; // d  (private — not exposed)
public:
    // p, q — two distinct primes
    RSA(unsigned p, unsigned q) : n_(p * q) {
        std::random_device rd;
        std::mt19937 rng(rd());

        unsigned long phi = euler_totient(p, q);
        unsigned long e   = choose_e(phi, rng);
        unsigned long d   = mod_inverse(e, phi);

        public_key_  = T(static_cast<int>(e));
        private_key_ = T(static_cast<int>(d));
    }

    // ── Public interface ──────────────────────────────────────────────────────

    unsigned getModulo() const { return n_; }

    T getPublicKey() const { return public_key_; }

    // Encrypt m with public key: c = m^e  (O(log e) multiplications)
    T encrypt(T m) const {
        return fast_pow(m, static_cast<unsigned long>(public_key_.to_int()), T(1));
    }

    // Decrypt s with private key: m = s^d  (O(log d) multiplications)
    T decrypt(T s) const {
        return fast_pow(s, static_cast<unsigned long>(private_key_.to_int()), T(1));
    }
};

#endif // RSA_HPP