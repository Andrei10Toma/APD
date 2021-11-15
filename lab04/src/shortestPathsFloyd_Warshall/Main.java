package shortestPathsFloyd_Warshall;

public class Main {

    public static void main(String[] args) {
        int M = 9;
        int cores = Runtime.getRuntime().availableProcessors();
        int[][] graph = {{0, 1, M, M, M},
                {1, 0, 1, M, M},
                {M, 1, 0, 1, 1},
                {M, M, 1, 0, M},
                {M, M, 1, M, 0}};

        // Parallelize me (You might want to keep the original code in order to compare)
        Thread[] thread = new Thread[cores];
        for (int i = 0; i < cores; i++) {
            int start = (int) (i * (double) graph.length / cores);
            int end = (int) Math.min((i + 1) * (double) graph.length / cores, graph.length);
            thread[i] = new Thread(new MyThread(i, start, end, graph));
            thread[i].start();
        }

        for (int i = 0; i < cores; i++) {
            try {
                thread[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }
    }
}
