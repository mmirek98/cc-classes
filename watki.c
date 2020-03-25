#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <linux/sched.h>
#include <sched.h>

#define _GNU_SOURCE
    #include <stdio.h>
       #include <stdlib.h>
       #include <errno.h>

int zmienna_wspolna = 0;

#define WYMIAR 1000
#define ROZMIAR WYMIAR*WYMIAR
double a[ROZMIAR], b[ROZMIAR], c[ROZMIAR];


double czasozajmowacz(){
  int i, j, k;
  int n = WYMIAR;
  for (i=0; i<ROZMIAR; i++) a[i]=1.0*i;

  for (i=0; i<ROZMIAR; i++) b[i]=1.0*(ROZMIAR-i);
  
  for (i=0; i<n; i++){
    for (j = 0; j<n; j++) {
      c[i+n*j] = 0.0;
      for (k=0; k<n; k++) {
	      c[i+n*j] += a[i+n*k] * b[k+n*j];
      }
    }
  }

  return(c[ROZMIAR-1]);
}

void * zadanie_watku (void * arg_wsk) {
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	printf("\twatek potomny: uniemozliwione zabicie\n");

	czasozajmowacz();

	printf("\twatek potomny: umozliwienie zabicia\n");
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	pthread_testcancel();

	zmienna_wspolna++;
	printf("\twatek potomny: zmiana wartosci zmiennej wspolnej\n");

	return(NULL);
}

void zmierz_czas(void* nr_watku) {
  // int s, j;
  // cpu_set_t cpuset;
  // pthread_t thread;
  
  // thread = pthread_self();

  // CPU_ZERO(&cpuset);
  // for (j = 0; j < 8; j++) {
  //   CPU_SET(j, &cpuset);
  // }

  // pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

  clock_t start = clock();
  czasozajmowacz();
  printf("Czas wykonywania: %lu ms dla watku %d \n", clock() - start, (int) nr_watku);
}

int main() {
  pthread_t tid[5];
  pthread_attr_t attr[5];

  int s;
  cpu_set_t cpuset;
  pthread_t thread;
  
  thread = pthread_self();

  CPU_ZERO(&cpuset);

  CPU_SET(2, &cpuset);

  pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);

  // ==== pthread_attr_setstacksize 
  int i, j;
  size_t stack_size[5];
  size_t size[5];
  stack_size[0] = PTHREAD_STACK_MIN;
  stack_size[1] = PTHREAD_STACK_MIN * 10;
  stack_size[2] = PTHREAD_STACK_MIN * 1000;
  stack_size[3] = PTHREAD_STACK_MIN * 100000;
  stack_size[4] = PTHREAD_STACK_MIN * PTHREAD_STACK_MIN;

  for (i = 0; i < 5; i++) {
    // pthread_attr_init(&attr[i]);
    // pthread_attr_setstacksize(&attr[i], stack_size[i]);
    // pthread_attr_getstacksize(&attr[i], &size[i]);
    // printf("Rozmiar stosu %d dla watku %d \n", size[i], i);

    pthread_create(&tid[i], NULL, (void*) zmierz_czas, (void*) i);
    // pthread_join(tid[i], NULL);
  }

  for (j = 0; j < 5; j++) 
    pthread_join(tid[j], NULL);
}
