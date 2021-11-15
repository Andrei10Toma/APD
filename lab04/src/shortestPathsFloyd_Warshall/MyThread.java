package shortestPathsFloyd_Warshall;

public class MyThread implements Runnable {
    private int id;
    private int start;
    private int end;
    private int[][] graph;

    public MyThread(int id, int start, int end, int[][] graph) {
        this.id = id;
        this.start = start;
        this.end = end;
        this.graph = graph;
    }

    @Override
    public void run() {
        for (int k = start; k < end; k++) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                }
            }
        }
    }

}
