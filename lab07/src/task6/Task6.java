package task6;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class Task6 extends RecursiveTask<Void> {
    private final int[] graph;
    private final int step;

    public Task6(int[] graph, int step) {
        this.graph = graph;
        this.step = step;
    }

    @Override
    protected Void compute() {
        if (Main.N == step) {
            Main.printQueens(graph);
            return null;
        }
        List<Task6> tasks = new ArrayList<>();
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (Main.check(newGraph, step)) {
                Task6 task = new Task6(newGraph, step + 1);
                tasks.add(task);
                task.fork();
            }
        }

        for (Task6 task : tasks) {
            task.join();
        }

        return null;
    }
}
