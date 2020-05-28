#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int availableProcesses, processNumber, range;
  int root = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &availableProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

  if (processNumber == 0) {
    printf("Podaj liczbę składników do zsumowania: ");
    scanf("%d", &range);
  }

	MPI_Bcast(&range, 1, MPI_INT, root, MPI_COMM_WORLD);

  double globalResult = 0, localResult = 0;
  int i;
  int chunk = range / availableProcesses;
  int rest = range % availableProcesses;

  if (rest != 0 && processNumber < rest) {
    chunk++;
  }

  for (i = chunk*processNumber; i < chunk*processNumber + chunk; i++) {
    localResult += pow(-1, i) / (2*i + 1);
  }

  printf("Dla procesora: %d suma wynosi: %lf; chunk=%d\n", processNumber, localResult, chunk);

  MPI_Reduce(&localResult, &globalResult, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  if (processNumber == 0) {
    printf("Wynik wynosi: %.15lf, wynik z bilioteki math: %.15lf\n", globalResult, M_PI_4);
  }

  MPI_Finalize();
  
  return 0;
}
