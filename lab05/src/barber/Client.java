package barber;

public class Client extends Thread {
    private final int id;

    public Client(int id) {
        super();
        this.id = id;
    }

    @Override
    public void run() {
        // TODO
        try {
            Main.chairsSemaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (Main.chairs > 0) {
            // client occupies a seat
            Main.chairs--;

            System.out.println("Client " + id + " is waiting for haircut");
            System.out.println("Available seats: " + Main.chairs);

            // TODO
            System.out.println("Client " + id + " is served by the barber");
            Main.clientsSemaphore.release();
            Main.chairsSemaphore.release();
            try {
                Main.barberReadySemaphore.acquire();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Main.leftClients[id] = Main.SERVED_CLIENT;
        } else {
            // TODO
            System.out.println("Client " + id + " left unserved");
            Main.leftClients[id] = Main.UNSERVED_CLIENT;
            Main.chairsSemaphore.release();
        }
    }
}
