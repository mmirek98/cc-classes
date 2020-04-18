#include <stdio.h>
#include <omp.h>

int main() {
  double sum = 0;
  int i, number = 13;

  #pragma omp parallel for num_threads(6) shared(sum)
  for (i = 0; i < 500; i++) {
    #pragma omp critical
    sum += number*number;
  }

  printf("suma: %f", sum);
}
