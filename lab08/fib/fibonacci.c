#include <stdio.h>
#include <omp.h>
#define CUTOFF 25

int fib(int n) {
    if (n == 0) {
      return 0;
    } else if (n == 1) {
      return 1;
    } else {
      return fib(n-1) + fib(n-2);
    }
}

int fibTasks(int n) {
  int prev1, prev2;

  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    #pragma omp task shared(prev1)
    prev1 = fib(n-1);

    #pragma omp task shared(prev2)
    prev2 = fib(n-2);
     
    #pragma omp taskwait
    return prev1 + prev2;
  }
}

int fibTasksOpt(int n) {
  int prev1, prev2;

  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else if (n < CUTOFF) {
    #pragma omp task shared(prev1)
    prev1 = fib(n-1);

    #pragma omp task shared(prev2)
    prev2 = fib(n-2);
     
    #pragma omp taskwait
    return prev1 + prev2;
  } else {
    return fibTasksOpt(n-1) + fibTasksOpt(n-2);
  }
}

int main() {
  int n = 45;
  double startTime = omp_get_wtime();
  fib(n);
  double endTime = omp_get_wtime() - startTime;
  printf("Fibonacci seq: %f\n", endTime);

  startTime = omp_get_wtime();
  #pragma omp parallel
  #pragma omp single nowait
  fibTasks(n);
  endTime = omp_get_wtime() - startTime;
  printf("Fibonacci tasks: %f\n", endTime);

  startTime = omp_get_wtime();
  #pragma omp parallel
  #pragma omp single nowait
  fibTasksOpt(n);
  endTime = omp_get_wtime() - startTime;
  printf("Fibonacci tasks: %f\n", endTime);
}
