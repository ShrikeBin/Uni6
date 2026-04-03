#ifndef RING_HPP
#define RING_HPP

template<int N>
class Ring {
    int value;

    static int normalize(int x) { return ((x % N) + N) % N; }

    static int extended_gcd(int a, int b, int &s) {
        int old_s = 1, ss = 0, old_r = a, r = b;
        while (r != 0) {
            int q = old_r / r;
            std::swap(old_r -= q * r, r);
            std::swap(old_s -= q * ss, ss);
        }
        s = old_s;
        return old_r; // gcd
    }

    static int inverse_val(int x) {
        int s;
        int g = extended_gcd(normalize(x), N, s);
        if (g != 1) throw std::domain_error("No inverse for " + std::to_string(x) + " mod " + std::to_string(N));
        return normalize(s);
    }

public:
    Ring(int v = 0) : value(normalize(v)) {}

    // Arithmetic
    Ring operator+(const Ring& o) const { return Ring(value + o.value); }
    Ring operator-(const Ring& o) const { return Ring(value - o.value); }
    Ring operator-()              const { return Ring(-value); }
    Ring operator*(const Ring& o) const { return Ring(value * o.value); }
    Ring operator/(const Ring& o) const {
        if (o.value == 0) throw std::domain_error("Division by zero mod " + std::to_string(N));
        return Ring(value * inverse_val(o.value));
    }

    // Compound assignment
    Ring& operator+=(const Ring& o) { return *this = *this + o; }
    Ring& operator-=(const Ring& o) { return *this = *this - o; }
    Ring& operator*=(const Ring& o) { return *this = *this * o; }
    Ring& operator/=(const Ring& o) { return *this = *this / o; }

    // Comparisons
    bool operator==(const Ring& o) const { return value == o.value; }
    bool operator!=(const Ring& o) const { return value != o.value; }
    bool operator< (const Ring& o) const { return value <  o.value; }
    bool operator<=(const Ring& o) const { return value <= o.value; }
    bool operator> (const Ring& o) const { return value >  o.value; }
    bool operator>=(const Ring& o) const { return value >= o.value; }

    // Streams
    friend std::ostream& operator<<(std::ostream& os, const Ring& r) {
        return os << r.value << " (mod " << N << ")";
    }
    friend std::istream& operator>>(std::istream& is, Ring& r) {
        int v; is >> v; r = Ring(v); return is;
    }

    int to_int() const { return value; }
};

#endif