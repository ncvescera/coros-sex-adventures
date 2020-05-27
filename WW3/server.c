#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"
#include "socket_name.h"


char *invert_letter_case(const char* stringa)
{
    int str_size = strlen(stringa);

    char *result = (char *) mcalloc(str_size, sizeof(char));

    for (int i = 0; i < str_size; i++)
    {
        if (islower(stringa[i]))    // se è lowecase
        {
            result[i] = (char) toupper(stringa[i]);
        } else
        {
           result[i] = (char) tolower(stringa[i]);
        }
    }

    return result;
}

void *handler(void *arg)
{
    int connessione = *(int *) arg; // prende l'argomento passatogli
    
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
            //_exit(err);
            return (void *) -1;
        }

        // controlla se il client si è disconnesso
        if (strcmp(buff, "quit") == 0)
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
            _exit(err);
        }
        
        // pulisce i buffer
        free(inverted);
        free(buff);
    }
    

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

int main(int argc, char const *argv[])
{
    // definizione dei valori per socket
    int server_fd;
    struct sockaddr_un address;
    

    strncpy(address.sun_path, SOCKET_NAME, UNIX_PATH_MAX);
    address.sun_family = AF_UNIX;

    /*
    struct sockaddr_un address; = {AF_UNIX, SOCKET_NAME};
    */

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
        printf("Client connesso %d\n", connessione);

        pthread_t pid;
        
        int result = pthread_create(&pid, NULL, &handler, (void *) &connessione);
        
        if (result != 0)
        {
            fprintf(stderr, "Creating thread\n");
            exit(EXIT_FAILURE);
        }

        /*
        int p = accept(server_fd, NULL, NULL);
        char tmp[] = "TESTO";

        sleep(2);

        write(p, tmp, strlen(tmp));

        close(p);
        sleep(1);
        */
    }
    /*
    char *test = str_input();

    char *inverted = invert_letter_case(test);
    
    printf("%s\n", inverted);
    
    free(inverted);
    free(test);
    */
    return EXIT_SUCCESS;
}
