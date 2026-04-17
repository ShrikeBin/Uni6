#ifndef RING_HPP
#define RING_HPP

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <string>

template<long long N>
class Ring {
    long long value;

    static long long normalize(long long x) { 
        return ((x % N) + N) % N; 
    }

    static long long extended_gcd(long long a, long long b, long long &s) {
        long long old_s = 1, ss = 0, old_r = a, r = b;
        while (r != 0) {
            long long q = old_r / r;
            long long tmp_r = r;
            r = old_r - q * r;
            old_r = tmp_r;

            long long tmp_s = ss;
            ss = old_s - q * ss;
            old_s = tmp_s;
        }
        s = old_s;
        return old_r;
    }

    static long long inverse_val(long long x) {
        long long s;
        long long g = extended_gcd(normalize(x), N, s);
        if (g != 1) throw std::domain_error("No inverse mod " + std::to_string(N));
        return normalize(s);
    }

public:
    Ring(long long v = 0) : value(normalize(v)) {}

    Ring operator+(const Ring& o) const { return Ring(value + o.value); }
    Ring operator-(const Ring& o) const { return Ring(value - o.value); }
    Ring operator-()              const { return Ring(-value); }
    Ring operator*(const Ring& o) const { return Ring(value * o.value); }
    Ring operator/(const Ring& o) const {
        if (o.value == 0) throw std::domain_error("Division by zero");
        return Ring(value * inverse_val(o.value));
    }

    Ring& operator+=(const Ring& o) { value = normalize(value + o.value); return *this; }
    Ring& operator-=(const Ring& o) { value = normalize(value - o.value); return *this; }
    Ring& operator*=(const Ring& o) { value = normalize(value * o.value); return *this; }

    bool operator==(const Ring& o) const { return value == o.value; }
    bool operator!=(const Ring& o) const { return value != o.value; }

    friend std::ostream& operator<<(std::ostream& os, const Ring& r) {
        return os << r.value << " [mod" << N << "]";
    }

    long long to_long() const { return value; }
    // Kept for RSA compatibility
    int to_int() const { return static_cast<int>(value); }
};

#endif