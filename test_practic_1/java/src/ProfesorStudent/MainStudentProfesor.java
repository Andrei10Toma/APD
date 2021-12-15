package ProfesorStudent;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;

class Professor implements Runnable {

    private final int professorId;
    private final Semaphore semaphoreProfessor;
    private final Semaphore semaphoreStudent;

    public Professor(int professorId, Semaphore semaphoreProfessor, Semaphore semaphoreStudent) {
        this.professorId = professorId;
        this.semaphoreProfessor = semaphoreProfessor;
        this.semaphoreStudent = semaphoreStudent;
    }


    @Override
    public void run() {

        try {
            semaphoreProfessor.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        boolean ok = true;
        for (int i = 1; i < MainStudentProfesor.P; ++i)
            if (!MainStudentProfesor.hasNapkin[i]) {
                ok = false;
                break;
            }

        if (ok)
            for(int i = 1; i < MainStudentProfesor.P; i++)
                semaphoreStudent.release();
    }
}

class Student implements Runnable {

    private final int studentId;
    private final Semaphore semaphoreProfessor;
    private final Semaphore semaphoreStudent;

    public Student(int studentId, Semaphore semaphoreProfessor, Semaphore semaphoreStudent) {
        this.studentId = studentId;
        this.semaphoreProfessor = semaphoreProfessor;
        this.semaphoreStudent = semaphoreStudent;
    }

    @Override
    public void run() {

        semaphoreProfessor.release();

        MainStudentProfesor.hasNapkin[studentId] = true;

        try {
            semaphoreStudent.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Studentul " + studentId + " a iesit din clasa!");
    }
}


public class MainStudentProfesor {
    public static int P;

    public static List<Thread> threadList = new ArrayList();

    public static boolean[] hasNapkin;

    public static void main(String[] args) {
        P = Integer.parseInt(args[0]);

        Semaphore semaphoreStudent = new Semaphore(0);
        Semaphore semaphoreProfessor = new Semaphore(-(P - 2));

        hasNapkin = new boolean[P + 1];

        Thread threadProfessor = new Thread(new Professor(0, semaphoreProfessor, semaphoreStudent));
        threadProfessor.start();

        threadList.add(threadProfessor);

        for (int i = 1; i < P; ++i) {
            Thread thread = new Thread(new Student(i, semaphoreProfessor, semaphoreStudent));
            threadList.add(thread);
            thread.start();
        }

        try {
            for (int i = 0; i < P; ++i)
                threadList.get(i).join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }
}
