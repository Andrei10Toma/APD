package schelett;

class MyThread implements Runnable {

    int id;

    MyThread(int id) {
        this.id = id;
    }

    @Override
    public void run() {
        int start = (int) (id * (double) Main.N / Main.P);
        int end = (int) Math.min((id + 1) * (double) Main.N / Main.P, Main.N);
        System.out.println(start + " " + end + " Hello world from " + id);
    }
}

public class Main {
    protected static int N;
    protected static int P;
    public static void main(String[] args) {
        N = 10;
        P = 5;
        Thread[] threads = new Thread[P];
        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i));
            threads[i].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
