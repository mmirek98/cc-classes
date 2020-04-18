#include <stdio.h>
#include <omp.h>

int main() {
  double sum = 0, startTime;
  int i, number = 13;

  startTime = omp_get_wtime();
  #pragma omp parallel for num_threads(6) shared(sum)
  for (i = 0; i < 500; i++) {
    #pragma omp critical
    sum += number*number;
  }
  printf("Czas dla OMP CRITICAL: %f\n", omp_get_wtime() - startTime);
  printf("Wynik: %f\n", sum);

  sum = 0;

  startTime = omp_get_wtime();
  #pragma omp parallel for num_threads(6) reduction(+ : sum)
  for (i = 0; i < 500; i++) {
    sum += number*number;
  }
  printf("Czas dla OMP REDUCTION: %f\n", omp_get_wtime() - startTime);
  printf("Wynik: %f\n", sum);

  sum = 0;

  omp_lock_t lock;
  omp_init_lock(&lock);
  startTime = omp_get_wtime();
  #pragma omp parallel for num_threads(6)
  for (i = 0; i < 500; i++) {
    omp_set_lock(&lock);
    sum += number*number;
    omp_unset_lock(&lock);
  }
  printf("Czas dla OMP LOCK: %f\n", omp_get_wtime() - startTime);
  printf("Wynik: %f\n", sum);


  sum = 0;

  startTime = omp_get_wtime();
  #pragma omp parallel for num_threads(6) ordered
  for (i = 0; i < 500; i++) {
    #pragma omp ordered
    sum += number*number;
  }
  printf("Czas dla OMP ORDERED: %f\n", omp_get_wtime() - startTime);
  printf("Wynik: %f\n", sum);

}
