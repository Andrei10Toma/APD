package task4;

import sampleForkJoinPool.Task;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class Task4 extends RecursiveTask<Void> {
    private final List<Integer> partialPath;
    private final int destination;

    public Task4(List<Integer> partialPath, int destination) {
        this.partialPath = partialPath;
        this.destination = destination;
    }

    @Override
    protected Void compute() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            return null;
        }

        List<Task4> tasks = new ArrayList<>();
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                Task4 task = new Task4(newPartialPath, destination);
                tasks.add(task);
                task.fork();
            }
        }

        for (Task4 task : tasks) {
            task.join();
        }

        return  null;
    }
}
