public class Main {

    public static void main(String[] args) {
        int cores = Runtime.getRuntime().availableProcessors();
        int i;
        Thread[] thread = new Thread[cores];
        for (i = 0; i < cores; i++) {
            thread[i] = new Thread(new MyThread1(i));
            thread[i].start();
        }

        for (i = 0; i < cores; i++) {
            try {
                thread[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public static class MyThread1 implements Runnable {
        private int id;

        public MyThread1(int id) {
            this.id = id;
        }

        public void setId(int id) {
            this.id = id;
        }

        public int getId() {
            return id;
        }

        @Override
        public void run() {
            System.out.println("Hello from thread " + id);
        }
    }
}
