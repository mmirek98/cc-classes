book.o: book.c
	mpicc book.c -o book.o

run: clean book.o
	mpirun -np $(np) ./book.o

clean:
	rm -f *.o