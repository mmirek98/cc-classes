#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stddef.h>
#include <string.h>

typedef struct Book_s {
  char title[255];
  char author[255];
  int pages;
  double price;
} Book;

int main(int argc, char* argv[]) {
  int availableProcesses, processNumber;
  const int messageTag = 13;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &availableProcesses);

  if (availableProcesses < 2) {
    printf("Potrzeba conajmniej dwóch procesów do komunikacji!\n");
    exit(-1);
  }

  const int propertiesNumber = 4;
  int blocksLen[4] = {255, 255, 1, 1};
  MPI_Datatype types[4] = {MPI_CHAR, MPI_CHAR, MPI_INT, MPI_DOUBLE};
  MPI_Datatype mpiBookType;
  MPI_Aint offsets[4];

  offsets[0] = offsetof(Book, title);
  offsets[1] = offsetof(Book, author);
  offsets[2] = offsetof(Book, pages);
  offsets[3] = offsetof(Book, price);

  MPI_Type_create_struct(propertiesNumber, blocksLen, offsets, types, &mpiBookType);
  MPI_Type_commit(&mpiBookType);

  MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

  if (processNumber == 0) {
    Book bookToSend;
    strncpy(bookToSend.title, "Mroczna Wieża", 255);
    strncpy(bookToSend.author, "Stephen King", 255);
    bookToSend.pages = 263;
    bookToSend.price = 39.99;
    const int destinationProcess = 1;

    MPI_Send(&bookToSend, 1, mpiBookType, destinationProcess, messageTag, MPI_COMM_WORLD);

    printf("Proces nr %d wysyła strukturę Book \n", processNumber);
  }

  if (processNumber == 1) {
    MPI_Status status;
    const int sourceProcess = 0;

    Book receivedBook;

    MPI_Recv(&receivedBook, 1, mpiBookType, sourceProcess, messageTag, MPI_COMM_WORLD, &status);
    printf("Proces %d: otrzymał książke '%s' autorstwa %s o liczbie stron %d kosztującej %.2f \n", processNumber, receivedBook.title, receivedBook.author, receivedBook.pages, receivedBook.price);
  }

  MPI_Type_free(&mpiBookType);
  MPI_Finalize();

  return 0;
}
