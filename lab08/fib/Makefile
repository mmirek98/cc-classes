CC = gcc
LD = gcc


OPT = -O3 -fopenmp
LIB = -lm

fibonacci: fibonacci.o
	$(LD) $(OPT) fibonacci.o -o fibonacci $(LIB)

fibonacci.o: fibonacci.c
	$(CC) -c $(OPT) fibonacci.c



clean:
	rm -f *.o
