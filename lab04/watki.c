#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define ITERATIONS 100000
pthread_mutex_t shield;

void increment(void* counter) {
	int i;
	for (i = 0; i<ITERATIONS; i++) {
    pthread_mutex_lock(&shield);
    (*(int*)counter)++;
    pthread_mutex_unlock(&shield);
  }
  pthread_exit(NULL);
}

void decrement(void* counter) {
	int i;
	for (i = 0; i<ITERATIONS; i++) {
    pthread_mutex_lock(&shield);
    (*(int*)counter)--;
    pthread_mutex_unlock(&shield);
  } 
  pthread_exit(NULL);
}

int main() {
  pthread_t tid[2];
	int counter = 0;

	printf("Counter before operations: %d\n", counter);
  pthread_mutex_init(&shield, NULL);
  pthread_create(&tid[0], NULL, (void*)increment, &counter);
  pthread_create(&tid[1], NULL, (void*)decrement, &counter);
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
	printf("Counter after operations: %d\n", counter);
}