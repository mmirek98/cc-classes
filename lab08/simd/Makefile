CC = gcc
LD = gcc


OPT = -O3 -fopenmp
LIB = -lm

wektory: wektory.o
	$(LD) $(OPT) wektory.o -o wektory $(LIB)

wektory.o: wektory.c
	$(CC) -c $(OPT) wektory.c -O3



clean:
	rm -f *.o
