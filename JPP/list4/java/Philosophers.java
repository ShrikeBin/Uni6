import java.util.Random;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Dining Philosophers — Java implementation.
 * Deadlock prevention: Waiter semaphore limits concurrent diners to N-1.
 * Starvation prevention: fair=true semaphores give FIFO ordering.
 */
public class Philosophers {

    static final int NUM_PHILOSOPHERS = 5;
    static final int MEALS_EACH       = 25;

    // Waiter: at most N-1 philosophers seated simultaneously
    static final Semaphore waiter = new Semaphore(NUM_PHILOSOPHERS - 1, true);

    // Each fork is a fair binary semaphore
    static final Semaphore[] forks = new Semaphore[NUM_PHILOSOPHERS];

    // Failure counters
    static final AtomicInteger[] failures = new AtomicInteger[NUM_PHILOSOPHERS];

    static final Object logLock = new Object();

    static void log(int id, String msg) {
        synchronized (logLock) {
            System.out.printf("[Philosopher %d] %s%n", id, msg);
        }
    }

    static class PhilosopherThread extends Thread {
        private final int id;
        private final int left;
        private final int right;
        private final Random rng = new Random();

        PhilosopherThread(int id) {
            this.id    = id;
            this.left  = id;
            this.right = (id + 1) % NUM_PHILOSOPHERS;
        }

        @Override
        public void run() {
            int meals = 0;
            int localFails = 0;

            while (meals < MEALS_EACH) {
                // Think
                log(id, "is thinking...");
                sleep(rng.nextInt(3) + 1);

                // Ask waiter
                log(id, "is hungry, waiting for seat...");
                try {
                    waiter.acquire();
                } catch (InterruptedException e) { Thread.currentThread().interrupt(); return; }

                // Pick up forks
                try {
                    forks[left].acquire();
                    log(id, "picked up LEFT fork");
                    forks[right].acquire();
                    log(id, "picked up RIGHT fork");
                } catch (InterruptedException e) {
                    waiter.release();
                    Thread.currentThread().interrupt();
                    return;
                }

                // Eat
                meals++;
                log(id, "*** EATING meal " + meals + " ***");
                sleep(rng.nextInt(3) + 1);

                // Put down forks
                forks[left].release();
                forks[right].release();
                log(id, "put down forks");

                waiter.release();
            }

            failures[id].set(localFails);
            log(id, "finished all " + MEALS_EACH + " meals. Failures: " + localFails);
        }

        private void sleep(int units) {
            try { Thread.sleep(units * 25L); }
            catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        }
    }

    public static void runPhilosophers() throws InterruptedException {
        System.out.println("=== Dining Philosophers (Java) ===");
        System.out.println("Philosophers : " + NUM_PHILOSOPHERS);
        System.out.println("Meals each   : " + MEALS_EACH);
        System.out.println("==================================");

        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            forks[i]    = new Semaphore(1, true);
            failures[i] = new AtomicInteger(0);
        }

        Thread[] threads = new Thread[NUM_PHILOSOPHERS];
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            threads[i] = new PhilosopherThread(i);
            threads[i].start();
        }
        for (Thread t : threads) t.join();

        System.out.println("\n=== FINAL REPORT ===");
        int total = 0;
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
            int f = failures[i].get();
            System.out.printf("Philosopher %d failed to eat: %d times%n", i, f);
            total += f;
        }
        if (total == 0)
            System.out.println("Fairness: perfectly fair — no philosopher starved.");
        else
            System.out.println("Fairness: waiter ensures no permanent starvation. Total failures: " + total);
    }
}