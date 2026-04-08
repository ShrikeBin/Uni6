#include "ring.hpp"
#include <iostream>

template<int N>
void test(int a, int b) {
    Ring<N> x(a), y(b);
    std::cout << "Testing with base " << N << ", a = " << a << ", b = " << b << ":\n";
    std::cout << "x:     "  << x        << "\n";
    std::cout << "y:     "  << y        << "\n";
    std::cout << "x + y: "  << x + y    << "\n";
    std::cout << "x - y: "  << x - y    << "\n";
    std::cout << "-x:    "  << -x       << "\n";
    std::cout << "x * y: "  << x * y    << "\n";
    try {
        std::cout << "x / y: " << x / y << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error during division: " << e.what() << "\n";
    }
    std::cout << "\n";
}

int main() {
    test<10>(3, 7);
    test<5>(2, 4);
    test<71>(36, 45);
    test<23>(13, 4);
    test<12>(5, 0);
    test<8>(5, 4);
    return 0;
}