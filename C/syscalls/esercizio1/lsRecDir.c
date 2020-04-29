// Scrivere un programma C che:
// - dato come argomento un nome di directory
// - visita ricorsivamente tutto il sottoalbero di directory che ha come radice la directory passata come argomento
// - Per ogni directory, il programma deve stampare sullo standard output le informazioni sui file trovati:
// -- "nome file" "size" "permessi"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

// dimenzione buffer per cwd
#define BUFFERSIZE 1024

/*Ritorna le info del file gestendo gli errori*/
struct stat get_file_info(const char* file) {
    struct stat info;

    if(stat(file, &info) == -1){
        perror("Getting file info ");
        exit(errno);
    }

    return info;
}

/*apre una cartella gestendo gli errori*/
DIR* open_dir(const char* path) {
    DIR* dir;

    if((dir = opendir(path)) == NULL) {
        perror("Opening dir ");
        exit(errno);
    }

    return dir;
}

/*chiude una cartella gestendo gli errori*/
void close_dir(DIR* dir) {
    if((closedir(dir)) == -1){
        perror("Closing dir ");
        exit(errno);
    }

    return;
}

/*Prende la current working directory (cwd) gestendo gli errori*/
char* get_cwd() {
    char* buffer = calloc(sizeof(char), BUFFERSIZE);

    if(getcwd(buffer, BUFFERSIZE) == NULL){
        perror("Getting cwd ");
        exit(errno);
    }

    return(buffer);
}

/*Cambia cartella gestendo gli errori*/
void change_dir(const char* path) {
    if(chdir(path) == -1){
        perror("Changing dir ");
        exit(errno);
    }

    return;
}

/*Concatena 2 stringhe per avere il path completo di un file*/
char* concat(const char* str1, const char* str2) {
    int str1_size = strlen(str1);
    int str2_size = strlen(str2);

    char *result = calloc(sizeof(char), (str1_size + str2_size + 1));

    strcat(result, str1);
    strcat(result, "/");
    strcat(result, str2);
    
    return(result);
}

void lsRecursiveDirectory(const char nomedirectory[]) {
    static int tab_numb = 0;    // server per indentare l'output

    struct stat info = get_file_info(nomedirectory);    // prende i dettagli del file
    char *cwd = get_cwd();  // salva la directory corrente per tornare indietro con la ricorsione

    // serve per indentare l'output
    for(int i = 0; i < tab_numb; i++)
        printf("\t");
    
    printf("%s ", nomedirectory);
    
    // controlla se il file passato è una cartella
    if(S_ISDIR(info.st_mode)) {
        printf("[dir]\n");

        // apertura della cartella
        DIR* dir = open_dir(nomedirectory);

        // entra nella cartella per continuare la ricorsione
        change_dir(nomedirectory);
        
        struct dirent* file;    // dati contenuto della cartella
        int count = 0;  // contatore per le cartelle da escludere
        while((errno = 0, file = readdir(dir)) != NULL) {
            // salta le cartelle . e ..
            if(count < 2) {
                count ++;
                continue;
            }

            tab_numb ++;    // serve per gestire l'output

            lsRecursiveDirectory(file->d_name);
            
            tab_numb --;    // serve per gestire l'output
        }

        // Gestione errori durante la lettura della cartella
        if(errno != 0) {
            perror("Getting file name ");
            exit(errno);
        }

        // chiusura della cartella
        close_dir(dir);

    }
    else {
        // stampa i permessi in modo leggibile
        printf((info.st_mode & S_IRUSR) ? "r" : "-");
        printf((info.st_mode & S_IWUSR) ? "w" : "-");
        printf((info.st_mode & S_IXUSR) ? "x" : "-");
        printf((info.st_mode & S_IRGRP) ? "r" : "-");
        printf((info.st_mode & S_IWGRP) ? "w" : "-");
        printf((info.st_mode & S_IXGRP) ? "x" : "-");
        printf((info.st_mode & S_IROTH) ? "r" : "-");
        printf((info.st_mode & S_IWOTH) ? "w" : "-");
        printf((info.st_mode & S_IXOTH) ? "x" : "-");

        printf(" %lub [file]\n", info.st_size);

    }

    // torna alla cartella precedente per poter tornare indietro con la ricorsione
    change_dir(cwd);

    return;
}

int main(int argc, char *argv[]) {
    if(argc == 2) {
        printf("La cartella da esplorare e': %s\n\n", argv[1]);

        lsRecursiveDirectory(argv[1]);
        return 0;

    } else {
        exit(EXIT_FAILURE);

    }
}   




// Passo 1: Controlliamo che l'argomento passato sia una directory
    // - man 2 stat
    // - vedi slide 32
    // - TEST:
    // -- ./main questaCartellaNonEsiste; echo $? # output atteso ENOENT == 2
    // -- ./main Makefile; echo $? # output atteso == EXIT_FAILURE (cat /usr/include/stdlib.h | grep "EXIT_FAILURE")
    // -- ./main test; echo $? # output atteso 0

    // Passo 2: Aprire la cartella
    // - vedi slide 36
    // - TEST: mkdir impossibleDir; sudo chmod -R 0 impossibleDir; ./main impossibleDir; echo $? # output atteso EACCES == 13
    // -- rimuovete poi la cartella con sudo rmdir

    // Passo 3: Lettura cartella (senza ricorsione)
    // - Stampare contenuto cartella
    // -- vedi slide 37-41
    // -- dividere tra cartelle e file
    // -- per i file stampare size e permessi
    // - Gestione chiusura cartella
    // - Suggerimenti:
    // -- definire una macro MAXSIZE (scegliete voi una grandezza) che verra' usata come max buffer per gestione stringhe
    // -- utilizzare strnpcy/strncat per definire correttamente una variabile "filename" per i file all'interno della cartella

    // Passo 4: ls ricorsivo
    // - Estendere passo 3 come segue:
    // -- nel caso in cui la variabile "filename" sia una cartella chiamare ricorsivamente lsRecursiveDirectory
    // - ATTENZIONE:
    // -- gestire directory . ..

    // Passo 5: Miglioriamo il nostro codice
    // - Definire una macro SYSCALL(nome, systemcall, stringa-errore-da-stampare, altri parametri)
    // -- tale macro deve eseguire systemcall, in caso di errore deve uscire con l'errore errno ritornato e stampare sullo standard-error la stringa-errore-da-stampare
    // -- decidere se e come poter utilizzare "altri paramentri"
    // - Gestire eventuale errore lunghezza buffer durante la manipolazione di stringhe