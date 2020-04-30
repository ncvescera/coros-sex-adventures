// Scrivere un programma C che:
// - riceve k argomenti
// - per ogni argomento:
// -- se sono file li stampa su stdandard output
// -- se sono cartelle stampa sullo standard output il contenuto di eventuali file trovati all'interno
// - NB: Non deve ricorsivamente cercare sottodirectory

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"

/* INUTILISSIMA
#define SYSCALL(return_type, syscall, str_err, check_err, ...) ({\
    return_type res = syscall(__VA_ARGS__);\
    if(res == check_err) {\
        perror(str_err);\
        -1;\
    }\
	else {\
    	res;\
	}\
}) */

void custom_cat(const char nomefile[])
{
	struct stat info = get_file_info(nomefile);

	if (errno == 0)
	{
		//controlla se il file è un file (non è una cartella)
		if (!S_ISDIR(info.st_mode))
		{
			//stampa il contenuto del file in stdout
			print_file(nomefile);
		}
		else
		{
			//controlla se ci sono file e li stampa in stdout
			print_dir(nomefile);
		}
	}

	errno = 0;
	printf("\n");

	return;
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		for (int i = 1; i < argc; ++i)
		{
			custom_cat(argv[i]);
		}

		return 0;
	}
	else
	{
		fprintf(stderr, "Inserisci più di un argomento!\n");
		exit(EXIT_FAILURE);
	}
}

// Passo 1: stat
// - Controllare se nomefile e' una directory o un file

// Passo 2: se e' un file
// - system call open per aprire il file
// - vedi slide 8-11
// - man 2 open
// - Riprendere syscall esercizio 1 modificandola come segue:
// -- togliere exit
// -- NB: tornare errno corretto

// Passo 3: read
// - Definire macro BUFSIZE (scegliete una grandezza che considerate appropriata)
// - Lettura file tramite system call read
// - vedi slide 12-13
// - Il programma che stiamo scrivendo vuole continuare a leggere la lista dei file anche nel caso di errori
// -- gestire errori
// -- attenzione alla terminazione della funzione

// Passo 4: write
// - Scrittura standard output tramite system call write
// - vedi slide 14-15
// - write deve essere richiamata tramite macro per standard output
// -- grep -A 3 "Standard file descriptors" /usr/include/unistd.h
// -- man 3 stdout
// - Il programma che stiamo scrivendo vuole continuare a leggere la lista dei file anche nel caso di errori
// -- gestire errori
// -- attenzione alla terminazione della funzione

// Passo 5: directories
// - recuperate del codice da esercizio 1 per questa parte
// - richiamate ricorsivamente la funzione custom_cat
// - NB: l'esercizio richiede che solo per le cartelle passate come argomento vengano stampati i file contenuti in essa
// -- gestite quindi eventuali sottocartelle

// Passo 6:
// come avete notato tra i due esercizi abbiamo riutilizzato pezzi di codice come SYSCALL o le define
// Provate a modificare il progetto come segue:
// - creare una cartella include allo stesso livello delle cartelle esercizio 1/2
// - scrivere un header include/"utilities.h" in cui includere parti del codice comuni come syscall, printerror, define ...
// - scrivere un unico makefile che (a differenza del target scelto) compila esercizio 1 o esercizio 2
// - NB: ricordatevi che ora esercizio 1 e 2 dipendono da "utilities.h"