leibniz-series.o: leibniz-series.c
	mpicc leibniz-series.c -lm -o leibniz-series.o

run: clean leibniz-series.o
	mpirun -np $(np) ./leibniz-series.o

clean:
	rm -f *.o