// User.java — DH party

import java.util.Random;

public class User {
    private final DHSetup setup;
    private final long    secret;
    private Ring          sharedKey = null;

    public User(DHSetup setup) {
        this.setup = setup;
        long p = setup.getFieldSize();
        this.secret = 2 + (Math.abs(new Random().nextLong()) % (p - 3));
    }

    public Ring getPublicKey() {
        return setup.power(setup.getGenerator(), secret);
    }

    public void setKey(Ring a) {
        this.sharedKey = setup.power(a, secret);
    }

    public Ring encrypt(Ring m) {
        ensureKeySet();
        return m.mul(sharedKey);
    }

    public Ring decrypt(Ring c) {
        ensureKeySet();
        return c.div(sharedKey);
    }

    private void ensureKeySet() {
        if (sharedKey == null)
            throw new IllegalStateException("DH User: key not set — call setKey() first");
    }
}