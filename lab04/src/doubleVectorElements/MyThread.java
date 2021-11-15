package doubleVectorElements;

public class MyThread implements Runnable {
    private int id;
    private int start;
    private int end;
    private int[] v;

    public MyThread(int id, int start, int end, int[] v) {
        super();
        this.id = id;
        this.start = start;
        this.end = end;
        this.v = v;
    }

    @Override
    public void run() {
        for (int i = start; i < end; i++) {
            v[i] = v[i] * 2;
        }
    }
}
