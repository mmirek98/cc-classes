#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define LENGTH 1000000

double *vec1, *vec2;

void prepareVectors() {
  int i;
  srand(time(NULL));
  vec1 = (double*)malloc(sizeof(double) * LENGTH);
  vec2 = (double*)malloc(sizeof(double) * LENGTH);

  for (i = 0; i < LENGTH; i++) {
    vec1[i] = rand();
    vec2[i] = rand();
  }
}

double multiplyVectors() {
  double sum = 0;
  int i;
  for (i = 0; i < LENGTH; i++) {
    sum += vec1[i] * vec2[i];
  }
  
  return sum;
}

double multiplyVectorsParallel() {
  int i;
  double sum = 0;

  #pragma omp parallel for reduction(+:sum)
  for (i = 0; i < LENGTH; i++)
    sum += vec1[i] * vec2[i];

  return sum;
}

double multiplyVectorsSIMD() {
  int i;
  double sum = 0;

  #pragma omp simd  
  for (i = 0; i < LENGTH; i++)
    sum += vec1[i] * vec2[i];

  return sum;
}

int main() {
  prepareVectors();
  double sum;
  double startTime = omp_get_wtime();
  sum = multiplyVectors();
  double endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors sequentially: %f\n", endTime);
  printf("Result SEQ = %f\n\n", sum);


  startTime = omp_get_wtime();
  sum = multiplyVectorsParallel();
  endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors parallel: %f\n", endTime);
  printf("Result Parallel = %f\n\n", sum);

  startTime = omp_get_wtime();
  sum = multiplyVectorsSIMD();
  endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors SIMD: %f\n", endTime);
  printf("Result SIMD = %f\n\n", sum);

}
