#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <math.h>

#define GAMMA 0.577216

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);

  int availableProcesses, processNumber, n;
  int root = 0;
  double *globalSeries, *localSeries;
  double *globalSum;

  MPI_Comm_size(MPI_COMM_WORLD, &availableProcesses);
  MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

  int *chunks = (int *)malloc(sizeof(int) * availableProcesses);
  int *offsets = (int *)malloc(sizeof(int) * availableProcesses);
  int currentOffset = 0;

  if (processNumber == 0) {
    printf("Podaj liczbę elementów: ");
    scanf("%d", &n);
    globalSeries = (double*)malloc(sizeof(double) * n);
    globalSum = (double*)malloc(sizeof(double) * availableProcesses);
    int i;
    for (i = 1; i < n; i++) {
      globalSeries[i] = (double)1/i;
    }

  }

	MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);

  int i;
  int defaultChunk = n / availableProcesses;
  for (i = 0; i < availableProcesses; i++) {
    chunks[i] = defaultChunk;
    int rest = n % availableProcesses;
    if (rest != 0 && i < rest) {
      chunks[i]++;
    }
    offsets[i] = currentOffset;
    currentOffset += chunks[i];
  }
  localSeries = (double*)malloc(sizeof(double)*chunks[processNumber]);

  MPI_Scatterv(globalSeries, chunks, offsets, MPI_DOUBLE, localSeries, chunks[processNumber], MPI_DOUBLE, 0, MPI_COMM_WORLD);

  double localSum = 0;
  for (i = 0; i < chunks[processNumber]; i++) {
    localSum += localSeries[i];
  }
  printf("Proc: %d, offset: %d, chunk=%d\n", processNumber, offsets[processNumber], chunks[processNumber]);

  MPI_Gather(&localSum, 1, MPI_DOUBLE, globalSum, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (processNumber == 0) {
    int i; double sum;
    for (i = 0; i < availableProcesses; i++) {
      sum += globalSum[i];
    }

    printf("Wartość ostateczna sumy: %lf\n", sum - log(n));
    printf("Różnica: %lf\n", fabs((sum - log(n)) - GAMMA));
  }

  MPI_Finalize();
  
  return 0;
}
