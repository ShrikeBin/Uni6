// TestMain.java — tests for RSA and Diffie-Hellman

public class TestMain {

    // ── RSA: n = 10007 * 10009 ────────────────────────────────────────────────
    static void testRSA() {
        System.out.println("══════════════════════════════════════");
        System.out.println("  RSA test  (ring mod 10007 * 10009)");
        System.out.println("══════════════════════════════════════");

        long p = 10007, q = 10009;
        long n = p * q;

        RSA rsa = new RSA(p, q);
        System.out.println("Modulus n  = " + rsa.getModulo());
        System.out.println("Public key = " + rsa.getPublicKey());
        System.out.println();

        long[] messages = {1, 42, 1234, 99999, n - 1};
        for (long msg : messages) {
            Ring m = Ring.of(n, msg);
            Ring c = rsa.encrypt(m);
            Ring d = rsa.decrypt(c);
            String ok = d.equals(m) ? "✓" : "✗ MISMATCH";
            System.out.printf("  m=%-12s  c=%-12s  d=%-12s  %s%n",
                    m.getValue(), c.getValue(), d.getValue(), ok);
        }
        System.out.println();
    }

    // ── Diffie-Hellman: field mod 1234567891 ──────────────────────────────────
    static void testDH() {
        System.out.println("═══════════════════════════════════════════════");
        System.out.println("  Diffie-Hellman test  (field mod 1234567891)");
        System.out.println("═══════════════════════════════════════════════");

        long p = 1234567891L;
        DHSetup setup = new DHSetup(p);
        System.out.println("Generator g = " + setup.getGenerator());
        System.out.println();

        User alice = new User(setup);
        User bob   = new User(setup);

        Ring aPub = alice.getPublicKey();
        Ring bPub = bob.getPublicKey();
        System.out.println("Alice public key = " + aPub.getValue());
        System.out.println("Bob   public key = " + bPub.getValue());
        System.out.println();

        alice.setKey(bPub);
        bob.setKey(aPub);

        long[] messages = {1, 7, 999, 123456};

        System.out.println("Alice encrypts, Bob decrypts:");
        for (long msg : messages) {
            Ring m = Ring.of(p, msg);
            Ring c = alice.encrypt(m);
            Ring d = bob.decrypt(c);
            String ok = d.equals(m) ? "✓" : "✗ MISMATCH";
            System.out.printf("  m=%-12s  c=%-12s  d=%-12s  %s%n",
                    m.getValue(), c.getValue(), d.getValue(), ok);
        }

        System.out.println("\nBob encrypts, Alice decrypts:");
        for (long msg : messages) {
            Ring m = Ring.of(p, msg);
            Ring c = bob.encrypt(m);
            Ring d = alice.decrypt(c);
            String ok = d.equals(m) ? "✓" : "✗ MISMATCH";
            System.out.printf("  m=%-12s  c=%-12s  d=%-12s  %s%n",
                    m.getValue(), c.getValue(), d.getValue(), ok);
        }

        System.out.println("\nError-handling test (encrypt without setKey):");
        try {
            User charlie = new User(setup);
            charlie.encrypt(Ring.of(p, 42));
            System.out.println("  ✗ Exception NOT raised — bug!");
        } catch (IllegalStateException e) {
            System.out.println("  ✓ Caught expected exception: " + e.getMessage());
        }
        System.out.println();
    }

    public static void main(String[] args) {
        testRSA();
        testDH();
    }
}