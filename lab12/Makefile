filter.o: main.c
	mpicc main.c -o filter.o

filter: clean filter.o
	mpirun -np $(np) ./filter.o $(in) $(out)

clean:
	rm -f *.o
