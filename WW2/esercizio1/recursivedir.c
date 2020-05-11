#include "recursivedir.h"

/*Ritorna le info del file gestendo gli errori*/
struct stat get_file_info(const char* file) {
    struct stat info;
    int err;

    if(stat(file, &info) == -1)
    {
        err = errno;    // perror potrebbe alterare errno
        perror("Getting file info ");
        exit(err);
    }

    return info;
}

/*Prende la current working directory (cwd) gestendo gli errori*/
char* get_cwd()
{
    char* buffer = calloc(sizeof(char), BUFFERSIZE);
    int err;

    if(getcwd(buffer, BUFFERSIZE) == NULL)
    {
        err = errno;
        perror("Getting cwd ");
        exit(err);
    }

    return(buffer);
}

/*Cambia cartella gestendo gli errori*/
void change_dir(const char* path)
{
    int err;

    if(chdir(path) == -1)
    {
        err = errno;
        perror("Changing dir ");
        exit(err);
    }

    return;
}

/*apre una cartella gestendo gli errori*/
DIR* open_dir(const char* path)
{
    DIR* dir;
    int err;

    if((dir = opendir(path)) == NULL)
    {
        err = errno;
        perror("Opening dir ");
        exit(err);
    }

    return dir;
}

/*chiude una cartella gestendo gli errori*/
void close_dir(DIR* dir)
{
    int err;

    if((closedir(dir)) == -1)
    {
        err = errno;
        perror("Closing dir ");
        exit(err);
    }

    return;
}

/* Ritrona un puntatore all'inizio dell'estenzione del file passato */
const char *get_file_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename)
    {
        return "";
    } else
    {
        return dot;
    }
}

/* Serve per ignorare le cartelle . e .. */
int isDot(const char dir[])
{
    return (strlen(dir) > 0 && dir[strlen(dir) - 1] == '.');
}

void lsRecursiveDirectory(const char *nomedirectory, const char *extention)
{
    static int tab_numb = 0;    // server per indentare l'output

    struct stat info = get_file_info(nomedirectory);    // prende i dettagli del file
    char *cwd = get_cwd();  // salva la directory corrente per tornare indietro con la ricorsione

    // serve per indentare l'output
    for(int i = 0; i < tab_numb; i++)
        printf("\t");
    
    //printf("%s ", nomedirectory);
    
    // controlla se il file passato è una cartella
    if(S_ISDIR(info.st_mode))
    {
        //printf("[dir]\n");

        // apertura della cartella
        DIR* dir = open_dir(nomedirectory);

        // entra nella cartella per continuare la ricorsione
        change_dir(nomedirectory);
        
        struct dirent* file;    // dati contenuto della cartella
        while((errno = 0, file = readdir(dir)) != NULL)
        {
            // salta le cartelle . e ..
            if (isDot(file->d_name))
            {
                continue;
            }

            lsRecursiveDirectory(file->d_name, extention);
            
        }

        // Gestione errori durante la lettura della cartella
        if(errno != 0)
        {
            perror("Getting file name ");
            exit(errno);
        }

        // chiusura della cartella
        close_dir(dir);

    }
    else
    {
        const char *ext = get_file_ext(nomedirectory);

        if (strcmp(ext, "") != 0)
        {
            if (strcmp(ext, extention) == 0)
            {
                printf("%s/", cwd);
                printf("%s", nomedirectory);
                printf(" %s", ctime(&(info.st_mtime))); // converte Epoch in Datetime (alla fine aggiunge \n)
            }
        }

    }

    // torna alla cartella precedente per poter tornare indietro con la ricorsione
    change_dir(cwd);

    return;
}