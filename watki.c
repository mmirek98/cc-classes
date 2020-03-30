#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ITERATIONS 100000

void increment(void* counter) {
	int i;
	for (i = 0; i < ITERATIONS; i++) (*(int*)counter)++;
}

void decrement(void* counter) {
	int i;
	for (i = 0; i < ITERATIONS; i++) (*(int*)counter)--;
}

int main() {
	pthread_t tid[2];
	int counter = 0;
		
	printf("Counter before operations: %d\n", counter);

  pthread_create(&tid[0], NULL, (void *)increment, &counter);
  pthread_create(&tid[1], NULL, (void *)decrement, &counter);
  
	printf("Counter after operations: %d\n", counter);
}