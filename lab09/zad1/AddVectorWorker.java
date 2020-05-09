package zad1;

class AddVectorWorker extends Thread {

  private int threadNum;

  public AddVectorWorker(int number) {
    this.threadNum = number;
    // System.out.println("Wątek " + number + " dostał porcji: " + chunk);
  }

  @Override
  public void run() {
    for (int i = threadNum; i < Main.vectorDimension; i += Main.threadNumber) {
      Main.resultVector[i] = Main.vector1[i] + Main.vector2[i];
    }
  }
}
