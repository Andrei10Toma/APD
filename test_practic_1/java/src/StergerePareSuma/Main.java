package StergerePareSuma;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

class MyThread implements Runnable {

    int id;

    MyThread(int id) {
        this.id = id;
    }

    @Override
    public void run() {
        int start = (int) (id * (double) Main.N / Main.P);
        int end = (int) Math.min((id + 1) * (double) Main.N / Main.P, Main.N);
        for (int i = start; i < end; i++) {
            if (Main.vector.get(i) % 2 != 0)
                Main.resultListInOrder.get(id).add(Main.vector.get(i));

        }
        try {
            Main.barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (id == 0) {
            int sum = 0;
            for (int i = 0; i < Main.resultListInOrder.size(); i++) {
                for (int j = 0; j < Main.resultListInOrder.get(i).size(); j++) {
                    sum += Main.resultListInOrder.get(i).get(j);
                    System.out.print(Main.resultListInOrder.get(i).get(j) + " ");
                }
            }
            System.out.println();
            System.out.println(sum);
        }
    }
}

public class Main {
    protected static int N;
    protected static int P;
    protected static List<Integer> vector;
    protected static List<Integer> resultList = new ArrayList<>();
    protected static List<List<Integer>> resultListInOrder = new ArrayList<>();
    protected static CyclicBarrier barrier;
    public static void main(String[] args) {
        P = 4;
        N = 10;
        barrier = new CyclicBarrier(P);
        vector = Arrays.asList(10, 12, 13, 15, 16, 20, 22, 25, 26, 29, 40, 42, 56, 75, 65, 65, 65);
        N = vector.size();
        Thread[] threads = new Thread[P];
        for (int i = 0; i < P; i++) {
            resultListInOrder.add(new ArrayList<>());
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
