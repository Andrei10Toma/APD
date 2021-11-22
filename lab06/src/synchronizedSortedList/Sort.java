package synchronizedSortedList;

import java.util.Collections;
import java.util.List;

public class Sort extends Thread {
    private final List<Integer> list;

    public Sort(List<Integer> list) {
        super();
        this.list = list;
    }

    @Override
    public void run() {
        try {
            Main.readSemaphore.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        Collections.sort(list);
    }
}
