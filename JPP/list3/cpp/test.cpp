#include <iostream>
#include <cassert>
#include "ring.hpp"
#include "rsa.hpp"
#include "diffie_hellman.hpp"

// ─── RSA test ────────────────────────────────────────────────────────────────
// n = 10007 * 10009 = 100 160 063
static constexpr int RSA_N = 10007 * 10009; // 100160063
using RingRSA = Ring<RSA_N>;

void test_rsa() {
    std::cout << "══════════════════════════════════════\n";
    std::cout << "  RSA test  (ring mod 10007 * 10009)  \n";
    std::cout << "══════════════════════════════════════\n";

    RSA<RingRSA> rsa(10007, 10009);
    std::cout << "Modulus n  = " << rsa.getModulo()   << "\n";
    std::cout << "Public key = " << rsa.getPublicKey() << "\n\n";

    // Test several messages
    int messages[] = {1, 42, 1234, 99999, 100160062};
    for (int msg : messages) {
        RingRSA m(msg);
        RingRSA c = rsa.encrypt(m);
        RingRSA d = rsa.decrypt(c);
        std::cout << "m=" << m << "  ->  c=" << c << "  ->  d=" << d;
        if (d == m) std::cout << "  ✓\n";
        else        std::cout << "  ✗ MISMATCH\n";
    }
    std::cout << "\n";
}

// ─── Diffie-Hellman test ──────────────────────────────────────────────────────
// Field characteristic: 1234567891 (prime)
static constexpr int DH_P = 1234567891;
using RingDH = Ring<DH_P>;

void test_dh() {
    std::cout << "══════════════════════════════════════════════\n";
    std::cout << "  Diffie-Hellman test  (field mod 1234567891) \n";
    std::cout << "══════════════════════════════════════════════\n";

    DHSetup<RingDH> setup(DH_P);
    std::cout << "Generator g = " << setup.getGenerator() << "\n\n";

    User<RingDH> alice(setup);
    User<RingDH> bob(setup);

    RingDH alice_pub = alice.getPublicKey();
    RingDH bob_pub   = bob.getPublicKey();

    std::cout << "Alice public key = " << alice_pub << "\n";
    std::cout << "Bob   public key = " << bob_pub   << "\n\n";

    // Key exchange
    alice.setKey(bob_pub);
    bob.setKey(alice_pub);

    // Test encryption/decryption
    int messages[] = {1, 7, 999, 123456, 1234567890};
    std::cout << "Encryption / decryption by Alice, verified by Bob:\n";
    for (int msg : messages) {
        RingDH m(msg);
        RingDH c  = alice.encrypt(m);
        RingDH d  = bob.decrypt(c);
        std::cout << "  m=" << m << "  ->  c=" << c << "  ->  d=" << d;
        if (d == m) std::cout << "  ✓\n";
        else        std::cout << "  ✗ MISMATCH\n";
    }

    std::cout << "\nEncryption / decryption by Bob, verified by Alice:\n";
    for (int msg : messages) {
        RingDH m(msg);
        RingDH c  = bob.encrypt(m);
        RingDH d  = alice.decrypt(c);
        std::cout << "  m=" << m << "  ->  c=" << c << "  ->  d=" << d;
        if (d == m) std::cout << "  ✓\n";
        else        std::cout << "  ✗ MISMATCH\n";
    }

    // Test error on missing setKey
    std::cout << "\nError-handling test (encrypt without setKey):\n";
    try {
        User<RingDH> charlie(setup);
        charlie.encrypt(RingDH(42));
        std::cout << "  ✗ Exception NOT raised — bug!\n";
    } catch (const std::logic_error& e) {
        std::cout << "  ✓ Caught expected exception: " << e.what() << "\n";
    }
    std::cout << "\n";
}

int main() {
    test_rsa();
    test_dh();
    return 0;
}