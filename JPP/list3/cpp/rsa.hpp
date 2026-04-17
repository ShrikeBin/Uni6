#ifndef RSA_HPP
#define RSA_HPP

#include "utils.hpp"
#include <stdexcept>

template<typename T>
class RSA {
    unsigned  n_;           
    T         public_key_;  // e
    T         private_key_; // d 

    static unsigned long long euler_totient(unsigned p, unsigned q) {
        return static_cast<unsigned long long>(p - 1) * (q - 1);
    }

    static unsigned long long choose_e(unsigned long long phi, std::mt19937 &rng) {
        std::uniform_int_distribution<unsigned long long> dist(2, phi - 1);
        unsigned long long e;
        long long x;
        do { 
            e = dist(rng); 
        } while (ext_gcd(static_cast<long long>(e), static_cast<long long>(phi), x) != 1);
        return e;
    }

    static unsigned long long mod_inverse(unsigned long long e, unsigned long long phi) {
        long long x;
        long long g = ext_gcd(static_cast<long long>(e), static_cast<long long>(phi), x);
        if (g != 1) throw std::domain_error("RSA: e has no inverse mod phi");
        return static_cast<unsigned long long>(((x % static_cast<long long>(phi)) + static_cast<long long>(phi)) % static_cast<long long>(phi));
    }

public:
    RSA(unsigned p, unsigned q) : n_(p * q) {
        std::random_device rd;
        std::mt19937 rng(rd());

        unsigned long long phi = euler_totient(p, q);
        unsigned long long e   = choose_e(phi, rng);
        unsigned long long d   = mod_inverse(e, phi);

        // Store keys as Ring elements
        public_key_  = T(static_cast<int>(e));
        private_key_ = T(static_cast<int>(d));
    }

    T encrypt(T m) const {
        // Exponent must be the raw integer value of the key
        return fast_pow(m, static_cast<unsigned long long>(public_key_.to_int()), T(1));
    }

    T decrypt(T s) const {
        // Exponent must be the raw integer value of the key
        return fast_pow(s, static_cast<unsigned long long>(private_key_.to_int()), T(1));
    }

    unsigned getModulo() const { return n_; }
    T getPublicKey() const { return public_key_; }
};

#endif