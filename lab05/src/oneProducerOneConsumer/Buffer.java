package oneProducerOneConsumer;

public class Buffer {
    private int a = -1;

    void put(int value) {
        synchronized (this) {
            while (a != -1) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            a = value;
            notify();
        }
    }

    int get() {
        synchronized (this) {
            while (a == -1) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            int aux = a;
            a = -1;
            notify();
            return aux;
        }
    }
}
