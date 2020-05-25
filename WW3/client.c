#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include "socket_name.h"
#include "utils.h"

#define MAX_INPUT_SIZE 256
#define UNIX_PATH_MAX 108


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

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    conn = connect(sock, (struct sockaddr *) &address, sizeof(address));

    char *BUFF;
    while (1)
    {
        
        BUFF = str_input();

        if (strcmp(BUFF, "quit") == 0)
        {
            free(BUFF);
            break;
        }

        write(sock, BUFF, strlen(BUFF));

        free(BUFF);
    }
    /*sleep(3);
    close(conn);
    close(sock);
    */
    return EXIT_SUCCESS;
}
