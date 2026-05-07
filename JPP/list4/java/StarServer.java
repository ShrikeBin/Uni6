import java.util.Random;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Star Server (hub-and-spoke messaging) — Java implementation.
 * Server has NO buffer: it relays each message before accepting the next.
 * Uses a SynchronousQueue per (server→user) channel and a single-slot
 * LinkedBlockingQueue (capacity=1) as the user→server channel to enforce
 * the "no-buffer" contract on the server side.
 */
public class StarServer {

    static final int NUM_USERS     = 10;
    static final int MSGS_PER_USER = 10;

    static final AtomicInteger[] received = new AtomicInteger[NUM_USERS];
    static final Object srvLogLock = new Object();

    static void srvLog(String who, String msg) {
        synchronized (srvLogLock) {
            System.out.printf("[%s] %s%n", who, msg);
        }
    }

    record Message(int from, int to, int seq) {}

    // Server thread: one message at a time, no internal buffer
    static class ServerThread extends Thread {
        private final SynchronousQueue<Message> inbox;          // from users
        private final SynchronousQueue<Message>[] outboxes;     // to users

        @SuppressWarnings("unchecked")
        ServerThread() {
            this.inbox   = new SynchronousQueue<>();
            this.outboxes = new SynchronousQueue[NUM_USERS];
            for (int i = 0; i < NUM_USERS; i++)
                this.outboxes[i] = new SynchronousQueue<>();
        }

        SynchronousQueue<Message> getInbox()                      { return inbox; }
        SynchronousQueue<Message> getOutbox(int userId)           { return outboxes[userId]; }

        @Override
        public void run() {
            int total   = NUM_USERS * MSGS_PER_USER;
            int count   = 0;
            srvLog("SERVER", "started, expecting " + total + " messages");
            try {
                while (count < total) {
                    Message m = inbox.take();           // blocks until a user sends
                    srvLog("SERVER", String.format(
                            "relaying msg from User%d to User%d (seq=%d)",
                            m.from(), m.to(), m.seq()));
                    outboxes[m.to()].put(m);            // blocks until recipient reads
                    received[m.to()].incrementAndGet();
                    count++;
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            srvLog("SERVER", "all messages delivered, shutting down.");
        }
    }

    static class UserThread extends Thread {
        private final int id;
        private final SynchronousQueue<Message> toServer;
        private final SynchronousQueue<Message> fromServer;
        private final CountDownLatch serverDone;
        private final Random rng = new Random();

        UserThread(int id,
                   SynchronousQueue<Message> toServer,
                   SynchronousQueue<Message> fromServer,
                   CountDownLatch serverDone) {
            this.id         = id;
            this.toServer   = toServer;
            this.fromServer = fromServer;
            this.serverDone = serverDone;
        }

        @Override
        public void run() {
            String name = "User" + id;
            srvLog(name, "started");

            // Sender sub-thread
            Thread sender = new Thread(() -> {
                try {
                    for (int seq = 1; seq <= MSGS_PER_USER; seq++) {
                        Thread.sleep((rng.nextInt(4) + 1) * 20L);
                        int dest = rng.nextInt(NUM_USERS);
                        srvLog(name, "sending msg #" + seq + " to User" + dest);
                        toServer.put(new Message(id, dest, seq));
                    }
                    srvLog(name, "finished sending.");
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });
            sender.setDaemon(true);
            sender.start();

            // Receiver loop — exits when server signals done
            try {
                while (!serverDone.await(0, TimeUnit.MILLISECONDS) ||
                       fromServer.peek() != null) {
                    Message m = fromServer.poll(50, TimeUnit.MILLISECONDS);
                    if (m != null)
                        srvLog(name, "received msg from User" + m.from() +
                               " (seq=" + m.seq() + ")");
                    if (serverDone.getCount() == 0 && fromServer.peek() == null) break;
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public static void runStarServer() throws InterruptedException {
        System.out.println("\n=== Star Server (Java) ===");
        System.out.println("Users         : " + NUM_USERS);
        System.out.println("Messages each : " + MSGS_PER_USER);
        System.out.println("=========================");

        for (int i = 0; i < NUM_USERS; i++)
            received[i] = new AtomicInteger(0);

        ServerThread srv = new ServerThread();
        CountDownLatch done = new CountDownLatch(1);

        // Wrap server to signal latch on completion
        Thread srvWrapper = new Thread(() -> {
            srv.run();
            done.countDown();
        });
        srvWrapper.start();

        Thread[] users = new Thread[NUM_USERS];
        for (int i = 0; i < NUM_USERS; i++) {
            users[i] = new UserThread(i, srv.getInbox(), srv.getOutbox(i), done);
            users[i].start();
        }

        srvWrapper.join();
        for (Thread u : users) u.join(2000); // give receivers time to drain

        System.out.println("\n=== FINAL REPORT ===");
        for (int i = 0; i < NUM_USERS; i++)
            System.out.printf("User %d received: %d message(s)%n", i, received[i].get());
    }
}