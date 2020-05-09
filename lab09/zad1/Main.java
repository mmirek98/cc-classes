package zad1;

import java.util.Random;

public class Main {
  private static final Random random = new Random();
  public static final int threadNumber = 4;
  public static final int vectorDimension = 10;
  public static int[] resultVector = new int[Main.vectorDimension];
  public static int[] vector1 = new int[Main.vectorDimension];
  public static int[] vector2 = new int[Main.vectorDimension];

  public static void main(String args[]) {
    AddVectorWorker[] threads = new AddVectorWorker[threadNumber];
    fillVector(vector1);
    fillVector(vector2);

    for (int i = 0; i < threadNumber; i++) {
      (threads[i] = new AddVectorWorker(i)).start();
    }

    for (int i = 0; i < threadNumber; i++) {
      try {
        threads[i].join();
      } catch (InterruptedException e) {
        System.out.println(e);
      }
    }

    printVector("=", resultVector);
  }

  private static void fillVector(int[] vec) {
    for (int i = 0; i < vectorDimension; i++) {
      vec[i] = random.nextInt(5);
      System.out.print(vec[i] + " ");
    }
    System.out.println();
  }

  private static void printVector(String name, int[] vec) {
    System.out.println(name);
    for (int i = 0; i < vectorDimension; i++) {
      System.out.print(vec[i] + " ");
    }
    System.out.println();
  }
}