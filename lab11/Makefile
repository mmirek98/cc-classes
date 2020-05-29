leibniz-series.o: leibniz-series.c
	mpicc leibniz-series.c -lm -o leibniz-series.o

leibniz: clean leibniz-series.o
	mpirun -np $(np) ./leibniz-series.o

gamma.o: gamma.c
	mpicc gamma.c -lm -o gamma.o

gamma: clean gamma.o
	mpirun -np $(np) ./gamma.o

clean:
	rm -f *.o