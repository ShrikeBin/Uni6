// User.java — DH party

import java.util.Random;

public class User {
    private final DHSetup setup;
    private final long    secret;
    private Ring          sharedKey = null;  // null until setKey() is called

    public User(DHSetup setup) {
        this.setup = setup;
        long p = setup.getFieldSize();
        // secret in [2, p-2]
        this.secret = 2 + (Math.abs(new Random().nextLong()) % (p - 3));
    }

    /** Returns g^secret */
    public Ring getPublicKey() {
        return setup.power(setup.getGenerator(), secret);
    }

    /** Computes shared key = a^secret */
    public void setKey(Ring a) {
        this.sharedKey = setup.power(a, secret);
    }

    /** Encrypt m by multiplying with shared key */
    public Ring encrypt(Ring m) {
        ensureKeySet();
        return m.mul(sharedKey);
    }

    /** Decrypt c by dividing by shared key */
    public Ring decrypt(Ring c) {
        ensureKeySet();
        return c.div(sharedKey);
    }

    private void ensureKeySet() {
        if (sharedKey == null)
            throw new IllegalStateException("DH User: key not set — call setKey() first");
    }
}