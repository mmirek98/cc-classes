#include <stdio.h>
#include <omp.h>

int main() {
  int i, chunkSize;

  // static, chunk = 3
  chunkSize = 3;
  printf("Przydział iteracji: STATYCZNY, rozmiar porcji: %d\n", chunkSize);
  #pragma omp parallel for num_threads(4) schedule(static, chunkSize) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }

  // static, chunk = default
  printf("Przydział iteracji: STATYCZNY, rozmiar porcji: DOMYŚLNY\n");
  #pragma omp parallel for num_threads(4) schedule(static) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }

  // dynamic, chunk = 3
  chunkSize = 3;
  printf("Przydział iteracji: DYNAMICZNY, rozmiar porcji: %d\n", chunkSize);
  #pragma omp parallel for num_threads(4) schedule(dynamic, chunkSize) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }

  // dynamic, chunk = default
  printf("Przydział iteracji: DYNAMICZNY, rozmiar porcji: DOMYŚLNY\n");
  #pragma omp parallel for num_threads(4) schedule(dynamic) 
  for (i = 0; i < 15; i++) {
    printf("Wątek: %d, i = %d\n", omp_get_thread_num(), i);
  }

}
