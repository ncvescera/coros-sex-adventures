CC = gcc
CFLAGS = -Wall -pedantic -std=c99

.PHONY: clean setup

all: main

main: main.o recursivedir.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c recursivedir.h
	$(CC) $(CFLAGS) -c $< -o $@

recursivedir.o: recursivedir.c recursivedir.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	- rm main *.o

setup :
	@ echo "Creazione cartelle per test"
	@ mkdir test test/A test/B test/A/C
	@ head -c 1KB /dev/zero > test/B/file1
	@ head -c 2KB /dev/zero > test/B/file2
	@ head -c 5KB /dev/zero > test/A/C/file3
	@ echo "Stampa:"
	@ ls -ahlR
