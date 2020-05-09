package zad2;

import java.util.ArrayList;
import java.util.Random;

public class Histogram {
  public static int N;
  public static int M;
  // in ASCII start from 33 (!)
  private final static int leftBound = 33;
  // cannot be greater than 93 (last sign in ASCII = 126 (~) (93 + 33 = 126))
  public static int charsNumber;
  public static char[][] histogram;

  public static ArrayList<Character> availableChars = new ArrayList<>();
  private final static Random generator = new Random();

  Histogram(int n, int m, int cNumber) {
    N = n;
    M = m;
    charsNumber = cNumber;
    histogram = new char[N][M];
    initCharArray();
    generateAndPrintHistogram();
  }

  private void initCharArray() {
    for (int i = 0; i < charsNumber; i++) {
      char pick = (char)(leftBound + generator.nextInt(charsNumber));
      if (!availableChars.contains(pick)) {
        availableChars.add(pick);
      } else {
        i--;
      }
    }
  }

  private void generateAndPrintHistogram() {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        int pick = generator.nextInt(charsNumber);
        histogram[i][j] = availableChars.get(pick);
        System.out.print(histogram[i][j] + " ");
      }
      System.out.println();
    }
  }

}