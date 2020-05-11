#include <stdio.h>
#include <stdlib.h>

#include "recursivedir.h"

/* Controlla se la stringa passata inizia con . */
int is_good_ext(const char *name)
{
    if (name[0] == '.')
    {
        return 1; // ritorna True
    } else
    {
        return 0; // ritorna False
    }
    
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Devi passare almeno un argomento\n");
        exit(EXIT_FAILURE);
    } else if (argc > 3)
    {
        fprintf(stderr, "Troppi argomenti passati\n");
        exit(EXIT_FAILURE);
    } else
    {
        char *path;

        // controllo se è una valida estenzione
        if ( ! is_good_ext(argv[1]) )
        {
            fprintf(stderr, "%s non e' un'estenzione valida\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        // controllo per capire su che cartella operare
        // [. se argc == 2, la cartella passata in input se argc == 3]
        if (argc == 2)
        {
            path = ".";
        } else
        {
            path = argv[2];
        }
        
        custom_find(path, argv[1]);

        return(EXIT_SUCCESS);
    }

}

