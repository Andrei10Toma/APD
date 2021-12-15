package task1;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnableTask1 implements Runnable {
    private final int destinationNode;
    private final ExecutorService executorService;
    private final AtomicInteger inQueue;
    private final List<Integer> partialPath;

    public MyRunnableTask1(int destinationNode, ExecutorService executorService, AtomicInteger inQueue, List<Integer> partialPath) {
        this.destinationNode = destinationNode;
        this.executorService = executorService;
        this.inQueue = inQueue;
        this.partialPath = partialPath;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destinationNode) {
            System.out.println(partialPath);
        } else {
            int lastNodeInPath = partialPath.get(partialPath.size() - 1);
            for (int[] ints : Main.graph) {
                if (ints[0] == lastNodeInPath) {
                    if (partialPath.contains(ints[1]))
                        continue;
                    inQueue.incrementAndGet();
                    ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                    newPartialPath.add(ints[1]);
                    executorService.submit(new MyRunnableTask1(destinationNode, executorService, inQueue, newPartialPath));
                }
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0)
            executorService.shutdown();
    }
}
