#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>

#include "socket_name.h"
#include "utils.h"


char *str_input()
{
    // inizializza la stringa
    char *tmp_str = (char *) mcalloc(MAX_INPUT_SIZE, sizeof(char));

    // prende in input la stringa
    fgets(tmp_str, MAX_INPUT_SIZE, stdin);

    // lunghezza della stringa
    int str_size = strlen(tmp_str);

    // elimina lo \n finale solo se la stringa non occupa tutto l'array
    if ((str_size + 1) != MAX_INPUT_SIZE)
    {
        tmp_str[str_size - 1] = '\0';
    }

    // sistema e rialloca la stringa
    tmp_str = (char *) mrealloc(tmp_str, str_size);

    return tmp_str;
}

int main(int argc, char const *argv[])
{
    int sock;
    int conn;

    struct sockaddr_un address;
    

    strncpy(address.sun_path, SOCKET_NAME, UNIX_PATH_MAX);
    address.sun_family = AF_UNIX;

    // creazione del socket e gestione errori
    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sock == -1)
    {
        int err = errno;
        perror("Creating socket");
        exit(err);
    }

    // connessione e gestione errori
    conn = connect(sock, (struct sockaddr *) &address, sizeof(address));

    if (conn != 0)
    {
        int err = errno;
        perror("Connecting");
        exit(err);
    }

    char *BUFF; // buffer per scrivere nello stream

    while (1)
    {
        // acquisizione stringa dall'utente
        BUFF = str_input();

        // scrittura nello stream e gestione degli errori
        int writed = write(sock, BUFF, strlen(BUFF));

        if (writed == -1)
        {
            int err = errno;
            perror("Writeing on stream");
            exit(err);
        }

        // se il client scrive "quit" si disconnette
        if (strcmp(BUFF, "quit") == 0)
        {
            free(BUFF);
            break;
        }

        // pulisce il buffer
        free(BUFF);
    }

    int err;

    // chiusura della connessione e gestione errori
    err = close(conn);
    
    if (err != 0)
    {
        err = errno;
        perror("Closing connection");
        exit(err);
    }

    // chiusura del socket e gestione degli errori
    err = close(sock);
    
    if (err != 0)
    {
        err = errno;
        perror("Closing socket");
        exit(err);
    }

    return EXIT_SUCCESS;
}
