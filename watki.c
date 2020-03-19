#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <linux/sched.h>
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

void wypisz_atrybuty(void* nr_watku) {
  int nr = (int) nr_watku;
  pthread_attr_t attr;
  pthread_t thread;
  size_t size;

  thread = pthread_self();

  pthread_getattr_np(thread, &attr);
  pthread_attr_getstacksize(&attr, &size);
  sleep(1);
  printf("Rozmiar stosu %d dla watku %d \n", size, nr);
}

int main() {
  pthread_t tid[10];
  pthread_attr_t attr[10];

  // ==== pthread_attr_setstacksize 
  int i, j;
  size_t stack_size[10];
  size_t size[10];
  stack_size[0] = PTHREAD_STACK_MIN;
  stack_size[1] = PTHREAD_STACK_MIN * 10;
  stack_size[2] = PTHREAD_STACK_MIN * 100;
  stack_size[3] = PTHREAD_STACK_MIN * 1000;
  stack_size[4] = PTHREAD_STACK_MIN * 10000;
  stack_size[5] = PTHREAD_STACK_MIN * 50000;
  stack_size[6] = PTHREAD_STACK_MIN * 100000;
  stack_size[7] = PTHREAD_STACK_MIN * 500000;
  stack_size[8] = PTHREAD_STACK_MIN * 1000000;
  stack_size[9] = PTHREAD_STACK_MIN * PTHREAD_STACK_MIN;

  for (i = 0; i < 10; i++) {
    pthread_attr_init(&attr[i]);
    pthread_attr_setstacksize(&attr[i], stack_size[i]);
    pthread_create(&tid[i], &attr[i], (void*) wypisz_atrybuty, (void*) i);
  }

  for (j = 0; j < 10; j++) 
    pthread_join(tid[j], NULL);
}
