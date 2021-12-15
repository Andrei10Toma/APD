package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnableTask3 implements Runnable {
    private final int[] graph;
    private final int step;
    private final ExecutorService executorService;
    private final AtomicInteger inQueue;

    public MyRunnableTask3(ExecutorService executorService, AtomicInteger inQueue, int[] graph, int step) {
        this.graph = graph;
        this.step = step;
        this.inQueue = inQueue;
        this.executorService = executorService;
    }

    @Override
    public void run() {
        if (Main.N == step) {
            Main.printQueens(graph);
        } else {
            for (int i = 0; i < Main.N; ++i) {
                int[] newGraph = graph.clone();
                newGraph[step] = i;

                if (Main.check(newGraph, step)) {
                    inQueue.incrementAndGet();
                    executorService.submit(new MyRunnableTask3(executorService, inQueue, newGraph, step + 1));
                }
            }
        }
        int left = inQueue.decrementAndGet();
        if (left == 0) {
            executorService.shutdown();
        }
    }
}
