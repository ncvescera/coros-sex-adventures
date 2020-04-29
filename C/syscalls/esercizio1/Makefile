CC = gcc
CFLAGS = -Wall -pedantic -std=c99

.PHONY: clean setup

# completare
all: main
	
main: lsRecDir.c
	$(CC) $(CFLAGS) $< -o $@

clean :
	-rm main

setup :
	@ echo "Creazione cartelle per test"
	@ mkdir test test/A test/B test/A/C
	@ head -c 1KB /dev/zero > test/B/file1
	@ head -c 2KB /dev/zero > test/B/file2
	@ head -c 5KB /dev/zero > test/A/C/file3
	@ echo "Stampa:"
	@ ls -ahlR

test1: main
	./main test || if [ $$? -eq 2 ]; then echo OK; else exit 1; fi;
	