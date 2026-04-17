// Ring.java — Modular arithmetic ring Z/nZ

public class Ring {
    private final long n;
    private final long value;

    public Ring(long n, long value) {
        this.n = n;
        this.value = normalize(value, n);
    }

    // ── Static factory ─────────────────────────────────────────────────────────
    public static Ring of(long n, long value) { return new Ring(n, value); }

    // ── Getters ────────────────────────────────────────────────────────────────
    public long getN()     { return n; }
    public long getValue() { return value; }

    // ── Helpers ────────────────────────────────────────────────────────────────
    private static long normalize(long v, long n) {
        return ((v % n) + n) % n;
    }

    private static long[] extGcd(long a, long b) {
        // returns {gcd, x} such that a*x + b*y = gcd
        long oldR = a, r = b;
        long oldX = 1, x = 0;
        while (r != 0) {
            long q   = oldR / r;
            long tmp = r;   r   = oldR - q * r;   oldR = tmp;
                  tmp = x;  x   = oldX - q * x;   oldX = tmp;
        }
        return new long[]{oldR, oldX};
    }

    // ── Arithmetic ─────────────────────────────────────────────────────────────
    public Ring add(Ring o)  { checkSameN(o); return new Ring(n, value + o.value); }
    public Ring sub(Ring o)  { checkSameN(o); return new Ring(n, value - o.value); }
    public Ring neg()        { return new Ring(n, -value); }
    public Ring mul(Ring o)  { checkSameN(o); return new Ring(n, value * o.value); }

    public Ring div(Ring o) {
        checkSameN(o);
        if (o.value == 0) throw new ArithmeticException("Division by zero mod " + n);
        return mul(o.inverse());
    }

    public Ring inverse() {
        long[] res = extGcd(value, n);
        if (res[0] != 1)
            throw new ArithmeticException("No inverse for " + value + " mod " + n);
        return new Ring(n, res[1]);
    }

    public boolean hasInverse() {
        return extGcd(value, n)[0] == 1;
    }

    // ── Comparisons ────────────────────────────────────────────────────────────
    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof Ring)) return false;
        Ring o = (Ring) obj;
        return n == o.n && value == o.value;
    }

    @Override public int hashCode() { return Long.hashCode(value) * 31 + Long.hashCode(n); }

    // ── Utility ────────────────────────────────────────────────────────────────
    @Override
    public String toString() { return value + " [mod " + n + "]"; }

    private void checkSameN(Ring o) {
        if (n != o.n) throw new IllegalArgumentException("Ring moduli differ: " + n + " vs " + o.n);
    }
}