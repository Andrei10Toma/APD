package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

public class Buffer {

    Queue<Integer> queue;
    Semaphore plin;
    Semaphore gol;

    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        plin = new Semaphore(0);
        gol = new Semaphore(size);
    }

    public void put(int value) {
        try {
            gol.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        synchronized (queue) {
            queue.add(value);
        }
        plin.release();
    }

    public int get() {
        try {
            plin.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        int a = -1;
        Integer result;
        synchronized (queue) {
            result = queue.poll();
        }
        gol.release();
        if (result != null) {
            a = result;
        }
        return a;
    }
}
