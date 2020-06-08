#include <mpi.h>
#include "pbm.h"

int main(int argc, char *argv[])
{
  image in;
  image out;

  int width;
  int height;

  // int maska[9] = {1, -2, 1, -2, 5, -2, 1, -2, 1};
  // int maska[9] = {3, 3, 3, 3, 1, 3, 3, 3, 3};
  int maska[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  readInput(argv[1], &in);

  out.height = in.height;
  out.width = in.width;
  out.maxValue = in.maxValue;
  memcpy(out.type, in.type, TYPE_LEN + 1);
  width = in.width;
  height = in.height;

  out.pixel = (uchar *)malloc(sizeof(uchar) * height * width);

  int npes;
  int myrank;
  double start, end;



  // int i,j;
  // for (i = 0; i < out.height * out.width; i++) {
  //   if (i <= out.width || i >= out.height * (out.width-1) ) {
  //     out.pixel[i] = in.pixel[i];
  //   }
  //   else if (i % out.width == 0 || i % (out.width-1) == 0) {
  //     out.pixel[i] = in.pixel[i];
  //   }
  //   else {
  //     int suma = 0;
  //     suma = maska[0] * in.pixel[i - in.width - 1] +
  //             maska[1] * in.pixel[i - in.width] +
  //             maska[2] * in.pixel[i - in.width + 1] +
  //             maska[3] * in.pixel[i - 1] +
  //             maska[4] * in.pixel[i] +
  //             maska[5] * in.pixel[i + 1] +
  //             maska[6] * in.pixel[i + in.width - 1] +
  //             maska[7] * in.pixel[i + in.width] +
  //             maska[8] * in.pixel[i + in.width + 1];
  //     out.pixel[i] = (unsigned char) (suma/9);
  //   }
  // }

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  printf("procek: %d, OUT: %p\n", myrank, &out);
  printf("procek: %d, IN: %p\n", myrank, &in);
  int *pixelChunks = (int *)malloc(sizeof(int) * npes);
  int *offsets = (int *)malloc(sizeof(int) * npes);

  int currentOffset = 0;
  int rest = (height*width) % npes;

  // wyliczanie chunka
  int i = 0;
  for (i = 0; i < npes; i++)
  {
    pixelChunks[i] = (height*width) / npes;

    if (rest > 0)
    {
      pixelChunks[i]++;
      rest--;
    }
    offsets[i] = currentOffset;
    currentOffset += pixelChunks[i];
  }

  if (myrank == 0)
  {
  }

  uchar *pixelsToCompute = (uchar *)malloc(sizeof(uchar) * pixelChunks[myrank]);
  MPI_Scatterv(in.pixel, pixelChunks, offsets, MPI_UNSIGNED_CHAR, pixelsToCompute, pixelChunks[myrank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  uchar *firstRow = (uchar*)malloc(sizeof(uchar)*width);
  uchar *lastRow = (uchar*)malloc(sizeof(uchar)*width);
  uchar *receivedUp = (uchar*)malloc(sizeof(uchar)*width);
  uchar *receivedDown = (uchar*)malloc(sizeof(uchar)*width);
  uchar *result = (uchar*)malloc(sizeof(uchar) * pixelChunks[myrank]);

  // if (myrank == 0 || myrank == npes-1) {
  //   result = (uchar*)malloc(sizeof(uchar) * (pixelChunks[myrank] + width));
  // } else {
  //   result = (uchar*)malloc(sizeof(uchar)* (pixelChunks[myrank] + 2*width));
  // }

  MPI_Status status;

  if (myrank != npes-1) {
    int lastRowStartIndex = pixelChunks[myrank] - width - 1;
    memcpy(lastRow, &pixelsToCompute[lastRowStartIndex], width);

    MPI_Send(&lastRow[0], width, MPI_UNSIGNED_CHAR, myrank+1, myrank+1, MPI_COMM_WORLD);
    printf("Proces %d wysyla swoj OSTATNI rzad do procesora %d\n", myrank, myrank+1);
  }

  if (myrank != 0) {
    MPI_Recv(&receivedUp[0], width, MPI_UNSIGNED_CHAR, myrank-1, myrank, MPI_COMM_WORLD, &status);
    printf("Proces %d otrzymuje rzad WYZEJ od procesora %d\n", myrank, myrank-1);
  }

  if (myrank != 0) {
    memcpy(firstRow, pixelsToCompute, width);

    MPI_Send(firstRow, width, MPI_UNSIGNED_CHAR, myrank - 1, myrank - 1, MPI_COMM_WORLD);
    printf("Proces %d wysyla swoj PIERWSZY rzad do procesora %d\n", myrank, myrank-1);

  }

  if (myrank != npes-1) {
    MPI_Recv(&receivedDown[0], width, MPI_UNSIGNED_CHAR, myrank + 1, myrank, MPI_COMM_WORLD, &status);
    printf("Proces %d otrzymuje rzad nizej od procesora %d\n", myrank, myrank+1);
  }


  // obliczanie
  if (myrank == 0) {
    for (i = 0; i < pixelChunks[myrank]; i++) {
      if (i <= width) {
        result[i] = pixelsToCompute[i];
      }
      else if (i % width == 0 || i % (width-1) == 0) {
        result[i] = pixelsToCompute[i];
      }
      else if (i >= pixelChunks[myrank]-width) {
        int sum = 0;
        sum = maska[0] * pixelsToCompute[i - width - 1] +
                maska[1] * pixelsToCompute[i - width] +
                maska[2] * pixelsToCompute[i - width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * receivedDown[i % width - 1] +
                maska[7] * receivedDown[i % width] +
                maska[8] * receivedDown[i % width + 1];
        result[i] = (uchar) (sum/9);
      }
      else {
        int sum = 0;
        sum = maska[0] * pixelsToCompute[i - width - 1] +
                maska[1] * pixelsToCompute[i - width] +
                maska[2] * pixelsToCompute[i - width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * pixelsToCompute[i + width - 1] +
                maska[7] * pixelsToCompute[i + width] +
                maska[8] * pixelsToCompute[i + width + 1];
        result[i] = (uchar) (sum/9);
      }
    }
  } else if (myrank == npes -1) {
    for (i = 0; i < pixelChunks[myrank]; i++) {
      if (i <= width) {
        int sum = 0;
        sum = maska[0] * receivedUp[i % width - 1] +
                maska[1] * receivedUp[i % width] +
                maska[2] * receivedUp[i % width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * pixelsToCompute[i + width - 1] +
                maska[7] * pixelsToCompute[i + width] +
                maska[8] * pixelsToCompute[i + width + 1];
        result[i] = (uchar) (sum/9);
        result[i] = pixelsToCompute[i];
      }
      else if (i >= pixelChunks[myrank]-width) {
        result[i] = pixelsToCompute[i];
      }
      else if (i % width == 0 || i % (width-1) == 0) {
        result[i] = pixelsToCompute[i];
      }
      else {
        int sum = 0;
        sum = maska[0] * pixelsToCompute[i - width - 1] +
                maska[1] * pixelsToCompute[i - width] +
                maska[2] * pixelsToCompute[i - width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * pixelsToCompute[i + width - 1] +
                maska[7] * pixelsToCompute[i + width] +
                maska[8] * pixelsToCompute[i + width + 1];
        result[i] = (uchar) (sum/9);
      }
    }
  } else {
    for (i = 0; i < pixelChunks[myrank]; i++) {
      
      if (i % width == 0 || i % (width-1) == 0) {
        result[i] = pixelsToCompute[i];
      }
      else if (i <= width) {
        int sum = 0;
        sum = maska[0] * receivedUp[i % width - 1] +
                maska[1] * receivedUp[i % width] +
                maska[2] * receivedUp[i % width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * pixelsToCompute[i + width - 1] +
                maska[7] * pixelsToCompute[i + width] +
                maska[8] * pixelsToCompute[i + width + 1];
        result[i] = (uchar) (sum/9);
        result[i] = pixelsToCompute[i];
      }
      else if (i >= pixelChunks[myrank]-width) {
        int sum = 0;
        sum = maska[0] * pixelsToCompute[i - width - 1] +
                maska[1] * pixelsToCompute[i - width] +
                maska[2] * pixelsToCompute[i - width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * receivedDown[i % width - 1] +
                maska[7] * receivedDown[i % width] +
                maska[8] * receivedDown[i % width + 1];
        result[i] = (uchar) (sum/9);
      }
      else {
        int sum = 0;
        sum = maska[0] * pixelsToCompute[i - width - 1] +
                maska[1] * pixelsToCompute[i - width] +
                maska[2] * pixelsToCompute[i - width + 1] +
                maska[3] * pixelsToCompute[i - 1] +
                maska[4] * pixelsToCompute[i] +
                maska[5] * pixelsToCompute[i + 1] +
                maska[6] * pixelsToCompute[i + width - 1] +
                maska[7] * pixelsToCompute[i + width] +
                maska[8] * pixelsToCompute[i + width + 1];
        result[i] = (uchar) (sum/9);
      }
    }
  }



  MPI_Gatherv(result, pixelChunks[myrank], MPI_UNSIGNED_CHAR, out.pixel, pixelChunks, offsets, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  MPI_Finalize();
  if (myrank == 0)
  {
    writeData(argv[2], &out);
    printf("end of process %lf", end - start);
  }
  return 0;
}
