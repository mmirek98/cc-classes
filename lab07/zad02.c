#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

int **A, **B, **result;
int m, n, p;

void multiplyMatrices();
void printMatrix(int **matrix, int rows, int columns);
void prepareMatrices();
void multiplyMatricesParallelOuter();
void multiplyMatricesParallelInner();

int main() {
  double startTime;

  prepareMatrices();
  printf("Dla rozmiarów macierzy: N = %d, M = %d, P = %d\n", n, m, p);

  // startTime = omp_get_wtime();
  // multiplyMatrices();
  // printf("Czas dla SYNCHRONICZNEGO: %f\n", omp_get_wtime() - startTime);
  // printMatrix(result, n, p);

  startTime = omp_get_wtime();
  multiplyMatricesParallelOuter();
  printf("Czas dla 4 WĄTKÓW - PĘTLA ZEWNĘTRZNA: %f\n", omp_get_wtime() - startTime);
// printMatrix(result, n, p);
  startTime = omp_get_wtime();
  multiplyMatricesParallelInner();
  printf("Czas dla 4 WĄTKÓW - PĘTLA WEWNĘTRZNA: %f\n", omp_get_wtime() - startTime);
// printMatrix(result, n, p);
}

void printMatrix(int **matrix, int rows, int columns) {
  int i,j;
  
  for (i = 0; i < rows; i++) {
    for (j = 0; j < columns; j++) {
      printf("%d\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n\n");
}

void prepareMatrices() {
  int i, j;
	srand(time(0));

  printf("Podaj rozmiar N:\n");
  scanf("%d", &n);

  printf("Podaj rozmiar M:\n");
  scanf("%d", &m);

  printf("Podaj rozmiar P:\n");
  scanf("%d", &p);

  // allocate for A
  A = (int **)malloc(n * sizeof(int *));
  for (i = 0; i < n; i++) {
    A[i] = (int *)malloc(m * sizeof(int));
  }

  // allocate for B
  B = (int **)malloc(m * sizeof(int *));
  for (i = 0; i < m; i++) {
    B[i] = (int *)malloc(p * sizeof(int));
  }

  // allocate for result
  result = (int **)malloc(n * sizeof(int *));
  for (i = 0; i < n; i++) {
    result[i] = (int *)malloc(p * sizeof(int));
  }


  // prepare values for array A
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++) 
      A[i][j] = rand()%100;
    
  // printf("A:\n");
  // printMatrix(A, n, m);


  // prepare values for array B
  for (i = 0; i < m; i++)
    for (j = 0; j < p; j++)
      B[i][j] = rand()%100;


  // printf("B:\n");
  // printMatrix(B, m, p);


  // fill with 0 result array 
  for (i = 0; i < n; i++)
    for (j = 0; j < p; j++)
      result[i][j] = 0;
}

void multiplyMatrices() {
  int i, j, k;
  int sum = 0;

  // multiply matrices
  for (i = 0; i < n; i++) {
    for (j = 0; j < p; j++) {
      sum = 0;
      for (k = 0; k < m; k++)
        sum += A[i][k] * B[k][j];

      result[i][j] = sum;
    }
  }
}

void multiplyMatricesParallelOuter() {
  int i, j, k;
  int sum = 0;
  
  // #pragma omp parallel for num_threads(4) private(sum, j, k) schedule(static)
  // #pragma omp parallel for num_threads(4) private(sum, j, k) schedule(static, 20) 
  // #pragma omp parallel for num_threads(4) private(sum, j, k) schedule(dynamic) 
  #pragma omp parallel for num_threads(4) private(sum, j, k) schedule(dynamic, 20) 
  for (i = 0; i < n; i++) {
    for (j = 0; j < p; j++) {
      sum = 0;
      for (k = 0; k < m; k++)
        sum += A[i][k] * B[k][j];

      result[i][j] = sum;
    }
  }
}

void multiplyMatricesParallelInner() {
  int i, j, k;
  int sum = 0;
  
  for (i = 0; i < n; i++) {
    // #pragma omp parallel for num_threads(4) private(sum, k) schedule(static)
    // #pragma omp parallel for num_threads(4) private(sum, k) schedule(static, 20) 
    // #pragma omp parallel for num_threads(4) private(sum, k) schedule(dynamic) 
    #pragma omp parallel for num_threads(4) private(sum, k) schedule(dynamic, 20)
    for (j = 0; j < p; j++) {
      sum = 0;
      for (k = 0; k < m; k++)
        sum += A[i][k] * B[k][j];

      result[i][j] = sum;
    }
  }
}