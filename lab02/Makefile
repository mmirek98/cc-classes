# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
# OPT = -O3

# domyślna kompilacja
all: program.o fork.o clone.o
	@echo ./fork.o to run example with fork, 
	@echo ./clone.o to run example with clone 

fork: fork.o
	$(LOADER) $(OPT) fork.o -o fork

clone: clone.o 
	$(LOADER) $(OPT) clone.o -o clone

# jak uzyskac plik clone.o ?
clone.o: clone.c pomiar_czasu.h program.o
	$(CCOMP) -Wall clone.c pomiar_czasu.c -o clone.o 

# jak uzyskac plik fork.o ?
fork.o: fork.c pomiar_czasu.h program.o
	$(CCOMP) -Wall fork.c pomiar_czasu.c -o fork.o

# jak uzyskac plik program.o ?
program.o: program.c
	$(CCOMP) program.c -o program.o

clean:
	rm -f *.o
