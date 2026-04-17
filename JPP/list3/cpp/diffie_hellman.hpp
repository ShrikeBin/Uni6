#ifndef DIFFIE_HELLMAN_HPP
#define DIFFIE_HELLMAN_HPP

#include <stdexcept>
#include <random>
#include <optional>

// Reuse fast_pow from rsa.hpp (include guard protects double-definition)
#ifndef RSA_HPP
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
#endif

// ─── DHSetup ─────────────────────────────────────────────────────────────────
// Randomly picks a generator g from [2, N-2] (N is the field characteristic).
// T must support: T(int), operator*, to_int().

template<typename T>
class DHSetup {
    T generator_;
    unsigned long field_size_; // characteristic (prime)
public:
    explicit DHSetup(unsigned long field_size) : field_size_(field_size) {
        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::uniform_int_distribution<unsigned long> dist(2, field_size - 2);
        generator_ = T(static_cast<int>(dist(rng)));
    }

    T getGenerator() const { return generator_; }

    // a^b using O(log b) multiplications
    T power(T a, unsigned long b) const {
        return fast_pow(a, b, T(1));
    }

    unsigned long getFieldSize() const { return field_size_; }
};

// ─── User ────────────────────────────────────────────────────────────────────
// Represents a party in the DH key exchange.

template<typename T>
class User {
    const DHSetup<T>&  setup_;
    unsigned long      secret_;
    std::optional<T>   shared_key_; // set after setKey()

public:
    explicit User(const DHSetup<T>& setup) : setup_(setup) {
        std::random_device rd;
        std::mt19937_64 rng(rd());
        // secret in [2, field_size - 2]
        std::uniform_int_distribution<unsigned long> dist(2, setup_.getFieldSize() - 2);
        secret_ = dist(rng);
    }

    // g^secret — the value this user sends to the other party
    T getPublicKey() const {
        return setup_.power(setup_.getGenerator(), secret_);
    }

    // Compute shared key = other_public^secret
    void setKey(T a) {
        shared_key_ = setup_.power(a, secret_);
    }

    // Encrypt by multiplying with shared key
    T encrypt(T m) const {
        if (!shared_key_.has_value())
            throw std::logic_error("DH User: key not set — call setKey() first");
        return m * (*shared_key_);
    }

    // Decrypt by dividing by shared key
    T decrypt(T c) const {
        if (!shared_key_.has_value())
            throw std::logic_error("DH User: key not set — call setKey() first");
        return c / (*shared_key_);
    }
};

#endif // DIFFIE_HELLMAN_HPP