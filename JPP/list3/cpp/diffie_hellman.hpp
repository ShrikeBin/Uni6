#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include "utils.hpp"
#include <stdexcept>
#include <optional>

template<typename T>
class DHSetup {
    T generator_;
    unsigned long long field_size_; 
public:
    explicit DHSetup(unsigned long long field_size) : field_size_(field_size) {
        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::uniform_int_distribution<unsigned long long> dist(2, field_size - 2);
        generator_ = T(static_cast<int>(dist(rng)));
    }

    T getGenerator() const { return generator_; }
    unsigned long long getFieldSize() const { return field_size_; }
    
    T power(T a, unsigned long long b) const {
        return fast_pow(a, b, T(1));
    }
};

template<typename T>
class User {
    const DHSetup<T>&  setup_;
    unsigned long long secret_;
    std::optional<T>   shared_key_;

public:
    explicit User(const DHSetup<T>& setup) : setup_(setup) {
        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::uniform_int_distribution<unsigned long long> dist(2, setup_.getFieldSize() - 2);
        secret_ = dist(rng);
    }

    T getPublicKey() const {
        return setup_.power(setup_.getGenerator(), secret_);
    }

    void setKey(T other_public) {
        shared_key_ = setup_.power(other_public, secret_);
    }

    T encrypt(T m) const {
        if (!shared_key_) throw std::logic_error("DH: Key not set");
        return m * (*shared_key_);
    }

    T decrypt(T c) const {
        if (!shared_key_) throw std::logic_error("DH: Key not set");
        
        // Find modular inverse of shared_key_ mod P
        long long x;
        long long g = ext_gcd(shared_key_->to_int(), setup_.getFieldSize(), x);
        if (g != 1) throw std::runtime_error("DH: Key has no inverse");
        
        unsigned long long inv = ((x % (long long)setup_.getFieldSize()) + setup_.getFieldSize()) % setup_.getFieldSize();
        return c * T(static_cast<int>(inv));
    }
};

#endif