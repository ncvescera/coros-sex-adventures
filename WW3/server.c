#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "utils.h"
#include "socket_name.h"

int server_fd;

char *invert_letter_case(const char* stringa);
void *handler(void *arg);
void signal_handler(int arg);
void cleanup();

int main(int argc, char const *argv[])
{
    // funzione che verrà invocata alla chiusura del programma
    int err = atexit(cleanup);

    if (err < 0)
    {
        err = errno;
        perror("On exit");
        exit(err);
    }
    
    // definizione dei valori per socket
    struct sockaddr_un address;
    
    strncpy(address.sun_path, SOCKET_NAME, UNIX_PATH_MAX);
    address.sun_family = AF_UNIX;

    // creazione del socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        int err = errno;
        perror("Creating socket");
        exit(err);
    }

    // binding del socket
    int bind_result = bind(server_fd, (struct sockaddr *) &address, sizeof(address));

    if (bind_result == -1)
    {
        int err = errno;
        perror("Binding socket");
        exit(err);
    }

    // listening
    int listen_result = listen(server_fd, SOMAXCONN);

    if (listen_result == -1)
    {
        int err = errno;
        perror("Listening");
        exit(err);
    }

    // inizializzazione degli handler per i segnali
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);


    // gestione delle connessioni
    while (1)
    {
        // accetta la nuova connessione
        int connessione = accept(server_fd, NULL, NULL);

        if (connessione < 0)
        {
            int err = errno;
            perror("Accepting connection");
            exit(err);
        }

        // Crea il thread che si occuperà della connessione
        pthread_t pid;
        
        int result = pthread_create(&pid, NULL, &handler, (void *) &connessione);
        
        if (result != 0)
        {
            fprintf(stderr, "Creating thread\n");
            exit(EXIT_FAILURE);
        }

        printf("Client connesso %d\n", connessione);
    }
    
    return EXIT_SUCCESS;
}

char *invert_letter_case(const char* stringa)
{
    int str_size = strlen(stringa);

    char *result = (char *) mcalloc(str_size, sizeof(char));

    for (int i = 0; i < str_size; i++)
    {
        if (isspace(stringa[i]))
        {
            // crea la stringa di errore
            result[0] = (char) 27;
            result[1] = '\0';

            return result;
        } else
        {
            if (islower(stringa[i]))    // se è lowecase
            {
                result[i] = (char) toupper(stringa[i]);
            } else
            {
                result[i] = (char) tolower(stringa[i]);
            }
        }
    }

    return result;
}

void *handler(void *arg)
{
    int connessione = *(int *) arg; // prende l'argomento passatogli
    
    // setta il thread come detach
    int err = pthread_detach(pthread_self());

    if (err != 0)
    {
        err = errno;
        perror("Setting thread attribute");
        return (void *) -1;
    }

    char *buff; // buffer per leggere lo stream

    while(1)
    {
        // alloca il buffer e controlla gli errori
        buff = (char *) mcalloc(MAX_INPUT_SIZE, sizeof(char));
 
        // legge dallo stream e controlla errori
        int readed = read(connessione, buff, MAX_INPUT_SIZE);

        if (readed < 0)
        {
            int err = errno;
            perror("Reading stream");

            return (void *) -1;
        }

        // controlla se il client si è disconnesso
        if (strncmp(buff, "quit", strlen("quit")) == 0)
        {
            free(buff);
            break;
        }

        // stampa cosa ha letto
        printf("%d: ", connessione);
        fflush(stdout);
        
        write(STDOUT_FILENO, buff, readed);
        
        printf("\n");
        fflush(stdout);

        // inverte la stringa e la rimanda al client
        char *inverted = invert_letter_case(buff);

        int writed = write(connessione, inverted, strlen(inverted));

        if (writed <= 0)
        {
            int err = errno;
            perror("Writeing on stream");

            return (void *) -1;
        }
        
        // pulisce i buffer
        free(inverted);
        free(buff);
    }
    
    // chiude la connessione
    int quit = close(connessione);

    if (quit != 0)
    {
        int err = errno;
        perror("Closing stream");
        _exit(err);
    }

    printf("Client %d disconnesso\n", connessione);

    return (void *) 0;
}

void signal_handler(int arg)
{
    cleanup();

    _exit(EXIT_SUCCESS);
}

void cleanup()
{
    int err;

    // chiusura della connessione e gestione errori
    err = close(server_fd);
    
    if (err != 0)
    {
        err = errno;
        perror("Closing connection");
        _exit(err);
    }

    err = unlink(SOCKET_NAME);

    if (err != 0)
    {
        err = errno;
        perror("Unlinking");
        _exit(err);
    }

    return;
}