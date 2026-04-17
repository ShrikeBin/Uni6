// RSA.java — RSA encryption/decryption over Ring<n>

import java.util.Random;

public class RSA {
    private final long n;          // p * q
    private final long publicKey;  // e
    private final long privateKey; // d  (kept private)

    public RSA(long p, long q) {
        this.n = p * q;
        long phi = (p - 1) * (q - 1);

        Random rng = new Random();
        long e;
        do {
            e = 2 + (Math.abs(rng.nextLong()) % (phi - 2));
        } while (gcd(e, phi) != 1);

        this.publicKey  = e;
        this.privateKey = modInverse(e, phi);
    }

    // ── Public interface ──────────────────────────────────────────────────────

    public long getModulo()     { return n; }
    public long getPublicKey()  { return publicKey; }

    /** Encrypt m using the public key: c = m^e mod n  (O(log e) multiplications) */
    public Ring encrypt(Ring m) {
        return fastPow(m, publicKey, Ring.of(n, 1));
    }

    /** Decrypt s using the private key: m = s^d mod n  (O(log d) multiplications) */
    public Ring decrypt(Ring s) {
        return fastPow(s, privateKey, Ring.of(n, 1));
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    /** Binary exponentiation: base^exp using O(log exp) multiplications */
    public static Ring fastPow(Ring base, long exp, Ring identity) {
        Ring result = identity;
        while (exp > 0) {
            if ((exp & 1) == 1) result = result.mul(base);
            base = base.mul(base);
            exp >>= 1;
        }
        return result;
    }

    private static long gcd(long a, long b) {
        while (b != 0) { long t = b; b = a % b; a = t; }
        return a;
    }

    private static long[] extGcd(long a, long b) {
        long oldR = a, r = b, oldX = 1, x = 0;
        while (r != 0) {
            long q = oldR / r;
            long t = r;   r = oldR - q * r;   oldR = t;
                  t = x;  x = oldX - q * x;   oldX = t;
        }
        return new long[]{oldR, oldX};
    }

    private static long modInverse(long e, long phi) {
        long[] res = extGcd(e, phi);
        if (res[0] != 1) throw new ArithmeticException("e has no inverse mod phi");
        return ((res[1] % phi) + phi) % phi;
    }
}