// DHSetup.java — Diffie-Hellman setup: random generator + fast power

import java.util.Random;

public class DHSetup {
    private final long p;         // prime field characteristic
    private final long generator;

    public DHSetup(long p) {
        this.p = p;
        Random rng = new Random();
        // generator in [2, p-2]
        this.generator = 2 + (Math.abs(rng.nextLong()) % (p - 3));
    }

    public long getFieldSize() { return p; }

    public Ring getGenerator() { return Ring.of(p, generator); }

    /** a^b using O(log b) multiplications */
    public Ring power(Ring a, long b) {
        Ring result = Ring.of(p, 1);
        Ring base   = a;
        long exp    = b;
        while (exp > 0) {
            if ((exp & 1) == 1) result = result.mul(base);
            base = base.mul(base);
            exp >>= 1;
        }
        return result;
    }
}