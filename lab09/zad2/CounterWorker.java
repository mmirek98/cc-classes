package zad2;

import java.util.Map;
import java.util.HashMap;

public class CounterWorker implements Runnable {
  private Map<Character, Integer> counter;
  private int threadId;

  CounterWorker(int threadId, Character[] assignedChars) {
    counter = new HashMap<>();
    this.threadId = threadId;

    for (char c : assignedChars) {
      counter.put(c, 0);
    }
  }

  @Override
  public void run() {
    findElements();
    printResult();
  }

  private void findElements() {
    for (int i = 0; i < Histogram.N; i++) {
      for (int j = 0; j < Histogram.M; j++) {
        char c = Histogram.histogram[i][j];
        if (counter.containsKey(c)) {
          counter.put(c, counter.get(c) + 1);
        }
      }
    }
  }

  private void printResult() {
    counter.forEach((character, count) -> System.out.println("Wątek " + threadId + ": " + character + " " + count + "x"));
  }

}