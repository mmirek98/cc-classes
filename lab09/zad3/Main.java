package zad3;

import java.util.Scanner;

public class Main {
  private static int n;
  private static final int threadNumber = 4;
  public static boolean[] primes;
  public static void main(final String args[]) {
    n = getUserInput();
    Thread threads[] = new Thread[n];
    initPrimes();
    int chunk = (n / threadNumber) - 1;
    int currentIndex = 2;

    for (int i = 0; i < threadNumber; i++) {
      if (i == 0) {
        int rest = n % threadNumber;
        threads[i] = new Thread(new SieveWorker(currentIndex, chunk + rest, i));
        currentIndex += chunk+rest;
      } else {
        threads[i] = new Thread(new SieveWorker(currentIndex + 1, chunk, i));
        currentIndex += chunk;
      }
      
      threads[i].start();
    }

    try {
      for (int i = 0; i < threadNumber; i++) {
        threads[i].join();
      }  
    } catch (InterruptedException e) {
      System.out.println(e);
    }

    printPrimes();
  }

  private static int getUserInput() {
    final Scanner scanner = new Scanner(System.in);
    System.out.println("Podaj n: ");
    int number = scanner.nextInt();
    scanner.close();
    return number;
  }

  private static void initPrimes() {
    primes = new boolean[n];
    primes[0] = false;
    primes[1] = false;
    for (int i = 2; i < n; i++) {
      primes[i] = true;
    }
  }

  private static void printPrimes() {
    for (int i = 2; i < n; i++) {
      if (primes[i])
        System.out.print(i + " ");
    }
  }
}