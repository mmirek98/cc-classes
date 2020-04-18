#include <stdio.h>
#include <omp.h>

int main() {
  int i, chunkSize;
  double startTimer;

  // static, chunk = 3
  chunkSize = 3;
  startTimer = omp_get_wtime();
  #pragma omp parallel for num_threads(4) schedule(static, chunkSize) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }
  printf("Przydział: STATYCZNY, Porcja: %d\n", chunkSize);
  printf("Czas: %f\n", omp_get_wtime() - startTimer);

  // static, chunk = default
  startTimer = omp_get_wtime();
  #pragma omp parallel for num_threads(4) schedule(static) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }
  printf("Przydział: STATYCZNY, Porcja: DOMYŚLNA\n");
  printf("Czas: %f\n", omp_get_wtime() - startTimer);

  // dynamic, chunk = 3
  chunkSize = 3;
  startTimer = omp_get_wtime();
  #pragma omp parallel for num_threads(4) schedule(dynamic, chunkSize) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }
  printf("Przydział: DYNAMICZNY, Porcja: %d\n", chunkSize);
  printf("Czas: %f\n", omp_get_wtime() - startTimer);

  // dynamic, chunk = default
  startTimer = omp_get_wtime();
  #pragma omp parallel for num_threads(4) schedule(dynamic) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }
  printf("Przydział: DYNAMICZNY, Porcja: DOMYŚLNA\n");
  printf("Czas: %f\n", omp_get_wtime() - startTimer);
}
