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

const int messagesPerProcessor = 10;


void prepareBook(Book *book) {
  strncpy(book->title, "Mroczna Wieża", 255);
  strncpy(book->author, "Stephen King", 255);
  book->pages = 263;
  book->price = 39.99;
}

void createMpiDatatypeDefault(MPI_Datatype *mpiBookType) {
  const int propertiesNumber = 4;
  int blocksLen[4] = {255, 255, 1, 1};
  MPI_Datatype types[4] = {MPI_CHAR, MPI_CHAR, MPI_INT, MPI_DOUBLE};
  MPI_Aint offsets[4];

  offsets[0] = offsetof(Book, title);
  offsets[1] = offsetof(Book, author);
  offsets[2] = offsetof(Book, pages);
  offsets[3] = offsetof(Book, price);

  MPI_Type_create_struct(propertiesNumber, blocksLen, offsets, types, mpiBookType);
  MPI_Type_commit(mpiBookType);
}

void RUN_DEFAULT() {
  double start, end;
  int availableProcesses, processNumber;
  MPI_Comm_size(MPI_COMM_WORLD, &availableProcesses);

  if (availableProcesses < 2) {
    printf("Potrzeba conajmniej dwóch procesów do komunikacji!\n");
    exit(-1);
  }

  MPI_Datatype mpiBookType;
  createMpiDatatypeDefault(&mpiBookType);

  MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

  if (processNumber == 0) {
    start = MPI_Wtime();
    Book bookToSend;
    prepareBook(&bookToSend);
    printf("Proces zerowy (nadawca) rozpoczyna komunikację i wysyła %d wiadomości z książką o tytule: %s \n",
      (availableProcesses-1)*messagesPerProcessor,
      bookToSend.title
    );
    int dstProcessor, messageNumb;

    for (dstProcessor = 1; dstProcessor < availableProcesses; dstProcessor++) {
      for (
        messageNumb = (dstProcessor-1) * messagesPerProcessor;
        messageNumb < dstProcessor * messagesPerProcessor;
        messageNumb++) {
          MPI_Send(&bookToSend, 1, mpiBookType, dstProcessor, messageNumb, MPI_COMM_WORLD);
      }
    }
  } else {
    MPI_Status status;
    Book receivedBook;
    int messageNumb;

    for (
      messageNumb = (processNumber-1) * messagesPerProcessor;
      messageNumb < processNumber * messagesPerProcessor;
      messageNumb++) {
        MPI_Recv(&receivedBook, 1, mpiBookType, 0, messageNumb, MPI_COMM_WORLD, &status);
        // printf("Proces %d: otrzymał książke '%s' autorstwa %s o liczbie stron %d kosztującej %.2f --> ID wiadomości: %d \n",
        //   processNumber,
        //   receivedBook.title,
        //   receivedBook.author,
        //   receivedBook.pages,
        //   receivedBook.price,
        //   messageNumb
        // );
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (processNumber == 0) {
    end = MPI_Wtime();
    printf("Czas operacji dla %d wiadomości: %lf\n", (availableProcesses-1)*messagesPerProcessor, end-start);
    exit(-1);
  }

  MPI_Type_free(&mpiBookType);
}

int createMpiDatatypePacked(char *buffer) {
  int propSize;
  int maxSize = 0;

  MPI_Pack_size(255, MPI_CHAR, MPI_COMM_WORLD, &propSize);
  maxSize += propSize;
  MPI_Pack_size(255, MPI_CHAR, MPI_COMM_WORLD, &propSize);
  maxSize += propSize;
  MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &propSize);
  maxSize += propSize;
  MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &propSize);
  maxSize += propSize;

  buffer = malloc(maxSize);
  return maxSize;
}

void RUN_PACKED() {
  double start, end;
  int availableProcesses, processNumber;
  MPI_Comm_size(MPI_COMM_WORLD, &availableProcesses);

  if (availableProcesses < 2) {
    printf("Potrzeba conajmniej dwóch procesów do komunikacji!\n");
    exit(-1);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &processNumber);

  if (processNumber == 0) {
    start = MPI_Wtime();
    int pos = 0;
    char *buffer;
    Book bookToSend;
    prepareBook(&bookToSend);
    int maxSize = createMpiDatatypePacked(buffer);

    printf("Proces zerowy (nadawca) rozpoczyna komunikację i wysyła %d wiadomości z książką o tytule: %s \n",
      (availableProcesses-1)*messagesPerProcessor,
      bookToSend.title
    );

    MPI_Pack(
      bookToSend.author,
      255,
      MPI_CHAR,
      buffer,
      maxSize,
      &pos,
      MPI_COMM_WORLD
    );

    MPI_Pack(
      bookToSend.title,
      255,
      MPI_CHAR,
      buffer,
      maxSize,
      &pos,
      MPI_COMM_WORLD
    );

    MPI_Pack(
      &bookToSend.pages,
      1,
      MPI_INT,
      buffer,
      maxSize,
      &pos,
      MPI_COMM_WORLD
    );

    MPI_Pack(
      &bookToSend.price,
      1,
      MPI_DOUBLE,
      buffer,
      maxSize,
      &pos,
      MPI_COMM_WORLD
    );

    int dstProcessor, messageNumb;

    for (dstProcessor = 1; dstProcessor < availableProcesses; dstProcessor++) {
      for (
        messageNumb = (dstProcessor-1) * messagesPerProcessor;
        messageNumb < dstProcessor * messagesPerProcessor;
        messageNumb++) {
          MPI_Send(buffer, 1, MPI_PACKED, dstProcessor, messageNumb, MPI_COMM_WORLD);
      }
    }
  } else {
    MPI_Status status;
    Book receivedBook;
    int messageNumb;

    char *buffer;
    int maxSize = createMpiDatatypePacked(buffer);

    for (
      messageNumb = (processNumber-1) * messagesPerProcessor;
      messageNumb < processNumber * messagesPerProcessor;
      messageNumb++) {
        int msgSize, pos = 0;
        MPI_Recv(buffer, maxSize, MPI_PACKED, 0, messageNumb, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_PACKED, &msgSize);

        MPI_Unpack(
          buffer,
          msgSize,
          &pos,
          receivedBook.author,
          255,
          MPI_CHAR,
          MPI_COMM_WORLD
        );

        MPI_Unpack(
          buffer,
          msgSize,
          &pos,
          receivedBook.title,
          255,
          MPI_CHAR,
          MPI_COMM_WORLD
        );

        MPI_Unpack(
          buffer,
          msgSize,
          &pos,
          &receivedBook.pages,
          1,
          MPI_INT,
          MPI_COMM_WORLD
        );

        MPI_Unpack(
          buffer,
          msgSize,
          &pos,
          &receivedBook.price,
          1,
          MPI_DOUBLE,
          MPI_COMM_WORLD
        );

        printf("Proces %d: otrzymał książke '%s' autorstwa %s o liczbie stron %d kosztującej %.2f --> ID wiadomości: %d \n",
          processNumber,
          receivedBook.title,
          receivedBook.author,
          receivedBook.pages,
          receivedBook.price,
          messageNumb
        );
    }
  }

  if (processNumber == 0) {
    end = MPI_Wtime();
    printf("Czas operacji dla %d wiadomości: %lf\n", (availableProcesses-1)*messagesPerProcessor, end-start);
  }
}

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);
  // RUN_DEFAULT();
  RUN_PACKED();
  MPI_Finalize();

  return 0;
}
