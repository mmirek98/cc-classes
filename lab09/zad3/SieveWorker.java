package zad3;

public class SieveWorker implements Runnable {
  private int from, length, id;

  SieveWorker(int from, int length, int id) {
    this.from = from;
    this.length = length;
    this.id = id;
    System.out.println("Wątek " + id + " szuka liczb w zakresie: " + from + "-" + (from+length));
  }

  @Override
  public void run() {
    for (int i = 2; i*i < (from+length); i++) {
      if (Main.primes[i]) {
        for (int j = i*i; j <= (from+length); j += i) {
          if (j < from) {
            continue;
          }

          Main.primes[j] = false;
        }
      }
    }
  }

}