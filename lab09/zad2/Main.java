package zad2;

import java.util.Scanner;

public class Main {
  private static int threadNumber;

  public static void main(String args[]) {
    Input input = (new Input()).get();
    threadNumber = input.threadsNum;
    Thread[] threads = new Thread[threadNumber];
    Histogram picture = new Histogram(input.n, input.m, input.charNum);
    int charChunk = Histogram.charsNumber / threadNumber;
    int rest = Histogram.charsNumber % threadNumber;

    int currentIndex = 0;
    for (int i = 0; i < threadNumber; i++) {
      int assignedNumber = charChunk;
      if (rest > 0) {
        assignedNumber++;
        rest--;
      }

      Character[] assignedChars = new Character[assignedNumber];
      System.arraycopy(
        Histogram.availableChars.toArray(new Character[0]), //from
        currentIndex, // fromIndex
        assignedChars, // to
        0, // startIndex
        assignedNumber // endIndex
      );

      (new Thread(new CounterWorker(i, assignedChars))).start();

      currentIndex += assignedNumber;
    }

    try {
      for (int i = 0; i < threadNumber; i++) {
        threads[i].join();
      }  
    } catch (InterruptedException e) {
      System.out.println(e);
    }
  }
}

class Input {
  int n;
  int m;
  int charNum;
  int threadsNum;

  Input() {
    Scanner scan = new Scanner(System.in);

    System.out.println("Podaj N (liczba wierszy): ");
    n = scan.nextInt();
    System.out.println("Podaj M (liczba kolumn): ");
    m = scan.nextInt();
    System.out.println("Podaj liczbę wątków do uruchomienia: ");
    threadsNum = scan.nextInt();
    System.out.println("Podaj liczbę znaków do wygenerowania obrazu (nie większa niż 93): ");
    charNum = scan.nextInt();

    scan.close();
  }

  Input get() {
    return this;
  }
}