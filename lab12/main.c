#include <mpi.h>
#include "pbm.h"

void computeFirstPart(uchar*, uchar*, uchar*, int, int);
void compute(uchar*, uchar*, uchar*, uchar*, int, int);
void computeLastPart(uchar*, uchar*, uchar*, int, int);
uchar validatePixel(int pixel);

const int filterMask[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
const int maskSum = 9;

int main(int argc, char *argv[])
{
  image in;
  image out;

  int width;
  int height;
  int npes;
  int myrank;
  double start, end;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &npes);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  if (myrank == 0)
  {
    readInput(argv[1], &in);

    out.height = in.height;
    out.width = in.width;
    out.maxValue = in.maxValue;
    memcpy(out.type, in.type, TYPE_LEN + 1);
    width = in.width;
    height = in.height;

    out.pixel = (uchar *)malloc(sizeof(uchar) * height * width);
  }

  MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

  int *pixelChunks = (int *)malloc(sizeof(int) * npes);
  int *offsets = (int *)malloc(sizeof(int) * npes);

  // calculate pixels chunk for each process
  int currentOffset = 0;
  int defaultPixelsChunk = (height * width) / npes;
  int restRows = (height) % npes;

  int i = 0;
  for (i = 0; i < npes; i++)
  {
    pixelChunks[i] = defaultPixelsChunk;
    offsets[i] = currentOffset;

    if (restRows > 0)
    {
      pixelChunks[i] += width;
      restRows--;
    }

    currentOffset += pixelChunks[i];
  }

  uchar *pixelsToCompute = (uchar *)malloc(sizeof(uchar) * pixelChunks[myrank]);

  MPI_Scatterv(in.pixel, pixelChunks, offsets, MPI_UNSIGNED_CHAR, pixelsToCompute, pixelChunks[myrank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  uchar *firstRow = (uchar *)malloc(sizeof(uchar) * width);
  uchar *lastRow = (uchar *)malloc(sizeof(uchar) * width);
  uchar *receivedUp = (uchar *)malloc(sizeof(uchar) * width);
  uchar *receivedDown = (uchar *)malloc(sizeof(uchar) * width);
  uchar *result = (uchar *)malloc(sizeof(uchar) * pixelChunks[myrank]);

  MPI_Status status;

  // request additional rows (to apply mask on horizontal edges)
  if (myrank != npes - 1)
  {
    int lastRowStartIndex = pixelChunks[myrank] - width - 1;
    memcpy(lastRow, &pixelsToCompute[lastRowStartIndex], width);

    MPI_Send(&lastRow[0], width, MPI_UNSIGNED_CHAR, myrank + 1, myrank + 1, MPI_COMM_WORLD);
    printf("Proces %d wysyla swoj OSTATNI rzad do procesora %d\n", myrank, myrank + 1);
  }

  if (myrank != 0)
  {
    MPI_Recv(&receivedUp[0], width, MPI_UNSIGNED_CHAR, myrank - 1, myrank, MPI_COMM_WORLD, &status);
    printf("Proces %d otrzymuje rzad WYZEJ od procesora %d\n", myrank, myrank - 1);
  }

  if (myrank != 0)
  {
    memcpy(firstRow, pixelsToCompute, width);

    MPI_Send(firstRow, width, MPI_UNSIGNED_CHAR, myrank - 1, myrank - 1, MPI_COMM_WORLD);
    printf("Proces %d wysyla swoj PIERWSZY rzad do procesora %d\n", myrank, myrank - 1);
  }

  if (myrank != npes - 1)
  {
    MPI_Recv(&receivedDown[0], width, MPI_UNSIGNED_CHAR, myrank + 1, myrank, MPI_COMM_WORLD, &status);
    printf("Proces %d otrzymuje rzad nizej od procesora %d\n", myrank, myrank + 1);
  }

  // compute pixels depending on process rank
  start = MPI_Wtime();
  if (myrank == 0)
  {
    computeFirstPart(result, pixelsToCompute, receivedDown, pixelChunks[myrank], width);
  }
  else if (myrank == npes - 1)
  {
    computeLastPart(result, pixelsToCompute, receivedUp, pixelChunks[myrank], width);
  }
  else
  {
    compute(result, pixelsToCompute, receivedUp, receivedDown, pixelChunks[myrank], width);
  }
  end = MPI_Wtime();

  MPI_Gatherv(result, pixelChunks[myrank], MPI_UNSIGNED_CHAR, out.pixel, pixelChunks, offsets, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  MPI_Finalize();

  if (myrank == 0)
  {
    writeData(argv[2], &out);
    printf("end of process %lf", end - start);
  }
  return 0;
}


uchar validatePixel(int pixel)
{
  if (pixel > 255)
  {
    pixel = 255;
  }
  else if (pixel < 0)
  {
    pixel = 0;
  }

  return (uchar)pixel;
}

void computeFirstPart(
    uchar *result,
    uchar *pixelsToCompute,
    uchar *receivedDown,
    int totalPixels,
    int width)
{
  int i;
  for (i = 0; i < totalPixels; i++)
  {
    int pixel;

    if (i <= width)
    {
      pixel = pixelsToCompute[i];
    }
    else if (i % width == 0 || i % (width - 1) == 0)
    {
      pixel = pixelsToCompute[i];
    }
    else if (i >= totalPixels - width)
    {
      int sum = 0;
      sum = filterMask[0] * pixelsToCompute[i - width - 1] +
            filterMask[1] * pixelsToCompute[i - width] +
            filterMask[2] * pixelsToCompute[i - width + 1] +
            filterMask[3] * pixelsToCompute[i - 1] +
            filterMask[4] * pixelsToCompute[i] +
            filterMask[5] * pixelsToCompute[i + 1] +
            filterMask[6] * receivedDown[i % width - 1] +
            filterMask[7] * receivedDown[i % width] +
            filterMask[8] * receivedDown[i % width + 1];

      pixel = (sum / maskSum);
    }
    else
    {
      int sum = 0;
      sum = filterMask[0] * pixelsToCompute[i - width - 1] +
            filterMask[1] * pixelsToCompute[i - width] +
            filterMask[2] * pixelsToCompute[i - width + 1] +
            filterMask[3] * pixelsToCompute[i - 1] +
            filterMask[4] * pixelsToCompute[i] +
            filterMask[5] * pixelsToCompute[i + 1] +
            filterMask[6] * pixelsToCompute[i + width - 1] +
            filterMask[7] * pixelsToCompute[i + width] +
            filterMask[8] * pixelsToCompute[i + width + 1];

      pixel = (sum / maskSum);
    }

    result[i] = validatePixel(pixel);
  }
}

void compute(
    uchar *result,
    uchar *pixelsToCompute,
    uchar *receivedUp,
    uchar *receivedDown,
    int totalPixels,
    int width)
{
  int i;
  for (i = 0; i < totalPixels; i++)
  {
    int pixel;

    if (i % width == 0 || i % (width - 1) == 0)
    {
      pixel = pixelsToCompute[i];
    }
    else if (i <= width) {
        int sum = 0;
        sum = filterMask[0] * receivedUp[i % width - 1] +
              filterMask[1] * receivedUp[i % width] +
              filterMask[2] * receivedUp[i % width + 1] +
              filterMask[3] * pixelsToCompute[i - 1] +
              filterMask[4] * pixelsToCompute[i] +
              filterMask[5] * pixelsToCompute[i + 1] +
              filterMask[6] * pixelsToCompute[i + width - 1] +
              filterMask[7] * pixelsToCompute[i + width] +
              filterMask[8] * pixelsToCompute[i + width + 1];

        pixel = (sum / maskSum);
    }
    else if (i >= totalPixels - width)
    {
      int sum = 0;
        sum = filterMask[0] * pixelsToCompute[i - width - 1] +
              filterMask[1] * pixelsToCompute[i - width] +
              filterMask[2] * pixelsToCompute[i - width + 1] +
              filterMask[3] * pixelsToCompute[i - 1] +
              filterMask[4] * pixelsToCompute[i] +
              filterMask[5] * pixelsToCompute[i + 1] +
              filterMask[6] * receivedDown[i % width - 1] +
              filterMask[7] * receivedDown[i % width] +
              filterMask[8] * receivedDown[i % width + 1];

      pixel = (sum / maskSum);
    }
    else
    {
      int sum = 0;
      sum = filterMask[0] * pixelsToCompute[i - width - 1] +
            filterMask[1] * pixelsToCompute[i - width] +
            filterMask[2] * pixelsToCompute[i - width + 1] +
            filterMask[3] * pixelsToCompute[i - 1] +
            filterMask[4] * pixelsToCompute[i] +
            filterMask[5] * pixelsToCompute[i + 1] +
            filterMask[6] * pixelsToCompute[i + width - 1] +
            filterMask[7] * pixelsToCompute[i + width] +
            filterMask[8] * pixelsToCompute[i + width + 1];

      pixel = (sum / maskSum);
    }

    result[i] = validatePixel(pixel);
  }
}

void computeLastPart(
    uchar *result,
    uchar *pixelsToCompute,
    uchar *receivedUp,
    int totalPixels,
    int width)
{
  int i;
  for (i = 0; i < totalPixels; i++)
  {
    int pixel;

    if (i <= width)
    {
      int sum = 0;
      sum = filterMask[0] * receivedUp[i % width - 1] +
            filterMask[1] * receivedUp[i % width] +
            filterMask[2] * receivedUp[i % width + 1] +
            filterMask[3] * pixelsToCompute[i - 1] +
            filterMask[4] * pixelsToCompute[i] +
            filterMask[5] * pixelsToCompute[i + 1] +
            filterMask[6] * pixelsToCompute[i + width - 1] +
            filterMask[7] * pixelsToCompute[i + width] +
            filterMask[8] * pixelsToCompute[i + width + 1];
      pixel = (sum / maskSum);
    }
    else if (i >= totalPixels - width)
    {
      pixel = pixelsToCompute[i];
    }
    else if (i % width == 0 || i % (width - 1) == 0)
    {
      pixel = pixelsToCompute[i];
    }
    else
    {
      int sum = 0;
      sum = filterMask[0] * pixelsToCompute[i - width - 1] +
            filterMask[1] * pixelsToCompute[i - width] +
            filterMask[2] * pixelsToCompute[i - width + 1] +
            filterMask[3] * pixelsToCompute[i - 1] +
            filterMask[4] * pixelsToCompute[i] +
            filterMask[5] * pixelsToCompute[i + 1] +
            filterMask[6] * pixelsToCompute[i + width - 1] +
            filterMask[7] * pixelsToCompute[i + width] +
            filterMask[8] * pixelsToCompute[i + width + 1];

      pixel = (sum / maskSum);
    }

    result[i] = validatePixel(pixel);
  }
}
