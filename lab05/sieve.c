#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <linux/sched.h>
#include <math.h>
#define TOTAL 1000

int prime[TOTAL];
pthread_mutex_t shield;

void sieve() {
  int prime[TOTAL];
  int i =0;
  for (i = 0; i < TOTAL; i++) {
    prime[i] = 1;
  }
  int j;

  for (j = 2; j*j < TOTAL; j++) {
    if (prime[j] == 1) {
      int k;
      for (k = j*j; k < TOTAL; k += j) {
        prime[k] = 0;
      }
    }
  }
  j = 0;

  for (j = 0; j <= TOTAL; j++) {
    if (prime[j] == 1) {
      printf("%d\n", j);
    }
  }
}

void* markNonPrimes(void* i) {
  int j = *((int*)i);
  
  pthread_mutex_lock(&shield);
  if (prime[j] == 1) {
    int k;
    for (k = j*j; k < TOTAL; k += j) {
    prime[k] = 0;
    pthread_mutex_unlock(&shield);

    }
  } else {
      pthread_mutex_unlock(&shield);
  }

  pthread_exit(NULL);
}

void sieveConc() {
  int i =0;
  for (i = 0; i < TOTAL; i++) {
    prime[i] = 1;
  }
  int j;

  int size = sqrt(TOTAL);
  pthread_t pool[size];
  int it = 0;
  pthread_mutex_init(&shield, NULL);

  for (j = 2; j*j < TOTAL; j++) {
    pthread_create(&pool[it], NULL, markNonPrimes, &j);
    pthread_join(pool[it], NULL);

    it++;
  }

  it = 0;

  j = 0;
  for (j = 0; j <= TOTAL; j++) {
    if (prime[j] == 1) {
      printf("%d\t", j);
    }
  }
}



void prepareArray() {
  int i;
  for (i = 0; i < TOTAL; i++) {
    prime[i] = 1;
  }
}
 
int main() {
  prepareArray();
  sieveConc();
   return 0;
}
 