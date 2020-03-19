#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *worker(void* ptr) {
  int thread_number = (int) ptr;
  printf("Wątek potomny nr: %d \n", thread_number);
  sleep(1);
}

int main() {
  pthread_t thread[10];
  int i, j, thread_result;

  for (i = 0; i < 10; i++) {
    thread_result = pthread_create(&thread[i], NULL, worker, (void *) i);
  }

  for (j = 0; j < 10; j++) {
    pthread_join(thread[j], NULL);
    printf("Dołączam wątek o nr: %d \n", j);
  }

  printf("Wątek głowny kończy działanie\n");
}