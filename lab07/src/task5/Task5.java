package task5;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class Task5 extends RecursiveTask<Void> {
    private final int[] colors;
    private final int step;

    public Task5(int[] colors, int step) {
        this.colors = colors;
        this.step = step;
    }

    @Override
    protected Void compute() {
        if (step == Main.N) {
            Main.printColors(colors);
            return null;
        }

        // for the node at position step try all possible colors
        List<Task5> tasks = new ArrayList<>();
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] = i;
            if (Main.verifyColors(newColors, step)) {
                Task5 task = new Task5(newColors, step + 1);
                tasks.add(task);
                task.fork();
            }
        }

        for (Task5 task : tasks) {
            task.join();
        }
        return null;
    }
}
