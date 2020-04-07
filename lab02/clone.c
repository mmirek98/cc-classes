#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/sched.h>
#include "pomiar_czasu.h"

int zmienna_globalna=0;
#define ROZMIAR_STOSU 1024*64
#define _GNU_SOURCE

int funkcja_watku(void* argument) {
  zmienna_globalna++;
  int wynik;
  wynik = execv("./program.o", NULL);

  if (wynik == -1)
    printf("Proces potomny nie wykonal programu\n");

  return 0;
}

int main() {
  void *stos;
  pid_t pid;
  int i; 
  stos = malloc(ROZMIAR_STOSU);

  if (stos == 0) {
    printf("Proces nadrzędny - blad alokacji stosu\n");
    exit( 1 );
  }

  inicjuj_czas();
  for (i = 0; i < 1000; i++) {

    pid = clone(&funkcja_watku, (void *)stos+ROZMIAR_STOSU, 
		 CLONE_FS | CLONE_FILES, 0);

    waitpid(pid, NULL, __WCLONE);
  }
  drukuj_czas();
  printf("Zmienna globalna: %d \n", zmienna_globalna);

  free(stos);
}
