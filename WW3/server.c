#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "utils.h"
#include "socket_name.h"
#include <unistd.h>
#include <pthread.h>

#define UNIX_PATH_MAX 108


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
    int connessione = *(int *) arg;;
   
    char *buff;

    while(1)
    {
        buff = (char *) mcalloc(1024, sizeof(char));
        
        read(connessione, buff, 1024);

        if (strcmp(buff, "quit") == 0)
        {
            free(buff);
            break;
        }

        printf("%s\n", buff);
        fflush(stdout);

        free(buff);
    }
    

    close(connessione);

    printf("Client disconnesso\n");
    fflush(stdout);

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

    while (1)
    {
        // dovrebbe creare un nuovo thread
        int connessione = accept(server_fd, NULL, NULL);

        if (connessione < 0)
        {
            int err = errno;
            perror("Accepting connection");
            exit(err);
        }

        printf("Client connesso\n");

        pthread_t pid;
        pthread_create(&pid, NULL, &handler, (void *) &connessione);
        
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
