package task2;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnableTask2 implements Runnable {
    private final ExecutorService executorService;
    private final AtomicInteger inQueue;
    private final int[] colors;
    int step;

    public MyRunnableTask2(ExecutorService executorService, AtomicInteger inQueue, int[] colors, int step) {
        this.executorService = executorService;
        this.inQueue = inQueue;
        this.colors = colors;
        this.step = step;
    }

    @Override
    public void run() {
        if (step == Main.N) {
            Main.printColors(colors);
        }
        else {
            // for the node at position step try all possible colors
            for (int i = 0; i < Main.COLORS; i++) {
                int[] newColors = colors.clone();
                newColors[step] = i;
                if (Main.verifyColors(newColors, step)) {
                    inQueue.incrementAndGet();
                    executorService.submit(new MyRunnableTask2(executorService, inQueue, newColors, step + 1));
                }
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            executorService.shutdown();
        }
    }
}
