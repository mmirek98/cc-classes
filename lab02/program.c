#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  printf("Michał Mirek, PID procesu: %d, PID rodzica: %d \n", getpid(), getppid());
}