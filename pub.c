#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h> 

int mugs;
int client;
int pubSize;
int consumingTime;
pthread_mutex_t beerShield;
pthread_t *clients;


void* drinkBeers() {
  int clientNumber = client;
  client++;
  int drunkBeers = 0;
  bool haveMug = false;
  for (drunkBeers = 0; drunkBeers < 2; ++drunkBeers) {
    while(!haveMug) {
      pthread_mutex_lock(&beerShield);
      if (mugs > 0) {
        mugs--;
        haveMug = true;
      }
      pthread_mutex_unlock(&beerShield);
    }

    switch (drunkBeers) {
      case 0:
        printf("Klient nr. %d PIJE swoje PIERWSZE piwerko!\n", clientNumber);
        break;
      case 1:
        printf("Klient nr. %d PIJE swoje DRUGIE piwo i jest porządnie wstawiony!\n", clientNumber);
        printf("Chwiejnym krokiem pub opuszcza klient nr. %d!\n", clientNumber);
        break;
    }

    usleep((rand()%consumingTime) * 1000000);
    pthread_mutex_lock(&beerShield);
    mugs++;
    haveMug = false;
    pthread_mutex_unlock(&beerShield);
  }
}

void openPub() {
  srand(time(NULL));
  size_t size;
  printf("Ile mamy kubków?: \t");
  scanf("%d", &mugs);
  printf("Ilu spodziewamy się klientów?: \t");
  scanf("%d", &pubSize);
  clients = malloc(sizeof(pthread_t) * pubSize + 1);
  printf("Ile zajmie jednemu klientowi wypicie jednego piwa (w sekundach)?: \t");
  scanf("%d", &consumingTime);
}


int main() {
  int i;
  openPub();

  for (i = 0; i < pubSize; i++) {
    usleep(250000);
    pthread_create(&clients[i], NULL, drinkBeers, NULL);
  }

  for (i = 0; i < pubSize; i++) {
    pthread_join(clients[i], NULL);
  }

  sleep(1);
  printf("ZAMYKAMY PUB\n");
}
