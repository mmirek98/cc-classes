# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
# OPT = -O3

# jak uzyskac plik watki.o ?
watki.o: watki.c
	gcc -pthread watki.c -o watki.o

clean:
	rm -f *.o