package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class Buffer {
	private final BlockingQueue<Integer> buf;

	public Buffer() {
	    this.buf = new ArrayBlockingQueue<>(5);
    }

	void put(int value) {
        try {
            buf.put(value);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

	int get() {
        try {
            return buf.take();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return -1;
    }
}
