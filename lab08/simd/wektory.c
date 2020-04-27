#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#define LENGTH 100000000

double *vec1, *vec2, *vec3;

void prepareVectors() {
  int i;
  srand(time(NULL));
  vec1 = (double*)malloc(sizeof(double) * LENGTH);
  vec2 = (double*)malloc(sizeof(double) * LENGTH);
  vec3 = (double*)malloc(sizeof(double) * LENGTH);

  for (i = 0; i < LENGTH; i++) {
    vec1[i] = rand() * 1000000000;
    vec2[i] = rand() * 1000000000;
  }
}

void multiplyVectors() {
  int i;
  for (i = 0; i < LENGTH; i++) {
    vec3[i] = vec1[i] * vec2[i];
  }
}

void multiplyVectorsParallel() {
  int i;

  #pragma omp parallel for
  for (i = 0; i < LENGTH; i++)
    vec3[i] = vec1[i] * vec2[i];
}

void multiplyVectorsSIMD() {
  int i;

  #pragma omp simd  
  for (i = 0; i < LENGTH; i++)
    vec3[i] = vec1[i] * vec2[i];
}

int main() {
  prepareVectors();

  double startTime = omp_get_wtime();
  multiplyVectors();
  double endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors sequentially: %f\n", endTime);


  startTime = omp_get_wtime();
  multiplyVectorsParallel();
  endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors parallel: %f\n", endTime);
  
  startTime = omp_get_wtime();
  multiplyVectorsSIMD();
  endTime = omp_get_wtime() - startTime;
  printf("MultiplyVectors SIMD: %f\n", endTime);

}
