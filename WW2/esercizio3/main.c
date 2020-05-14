#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"


// inizializza il mutex per la cabina
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

// CV per centro e stazione
static pthread_cond_t stazione = PTHREAD_COND_INITIALIZER;
static pthread_cond_t centro_storico = PTHREAD_COND_INITIALIZER;

//static int cond = 1;
static const int POSTI_MAX = 4;
static int passeggeri = POSTI_MAX;

static void *cabina(void *arg)
{
    int posizione = 0;
    //int *myid = (int *) arg;
    
    while (1)
    {
        pthread_cond_t *local_posizione;

        if (posizione == 0)
        {
            local_posizione = &stazione;
        } else
        {
            local_posizione = &centro_storico;
        }

        thread_cond_broadcast(local_posizione);

        while (passeggeri > 0)
        {
            sleep(1);
            continue;
        }

        printf("Passeggeri: %d\n", passeggeri);

        // blocca la risorsa dagli altri thread
        thread_mutex_lock(&mtx);

        printf("PARTENZA ...\n");
        sleep(2);
        printf("Arrivo!\n");
        passeggeri = POSTI_MAX;

        // inversione della stazione
        if (posizione == 0)
        {
            posizione = 1;
        } else
        {   
            posizione = 0;
        }

        thread_mutex_unlock(&mtx);

        //sleep(2);
    }

    return((void *) 0);
}

static void *turista(void *arg)
{
    init_data *data = (init_data *) arg;
    pthread_t *id = data->tid;
    int posizione = data->posizione;

    // sezione critica
    //thread_mutex_lock(&mtx);
    while (1)
    {
        pthread_cond_t *local_posizione;

        if (posizione == 0)
        {
            local_posizione = &stazione;
            
        } else
        {   
            local_posizione = &centro_storico;
        }

        thread_cond_wait(local_posizione, &mtx);

        // prende posto nel minimetro
        if (passeggeri)
        {
            passeggeri --;

            printf("ID: %ld sale\n", *id);

            // libera la risorsa
            thread_mutex_unlock(&mtx);

            // inversione della stazione
            if (posizione == 0)
            {
                posizione = 1;
            } else
            {   
                posizione = 0;
            }

            sleep(2);

        } else
        {
            thread_mutex_unlock(&mtx);
        }
    }
    return((void *) 0);
}


#define NTURISTI 5

int main(int argc, char const *argv[])
{
    /*
    pthread_t minimetro;
    int err;
    int status;

    err = pthread_create(&minimetro, NULL, &cabina, (void *) &minimetro);

    if (err != 0)
    {
        perror("Creating T1");
        exit(EXIT_FAILURE);
    } 
    
    printf("IO SONO IL MAIN\n");

    sleep(10);
    cond = 0;
    pthread_join(minimetro, (void *) &status);
    printf("T1: %d\n", status);*/

    // Inizializzazione Turisti
    pthread_t turisti[5];

    for (int i = 0; i < NTURISTI; i++)
    {
        init_data *data = malloc(sizeof(init_data));
        data->tid = &turisti[i];
        data->posizione = 0;

        thread_create(&turisti[i], NULL, &turista, (void *) data);
        printf("ID: %ld ", turisti[i]);
    }

    printf("\n\n");

    // Inizializzazione Mini
    pthread_t mini;

    thread_create(&mini, NULL, &cabina, &mini);

    // Attesa della fine dei thread
    thread_join(mini, NULL);
    for (int i = 0; i < NTURISTI; i++)
    {
        thread_join(turisti[i], NULL);
    }

    /*
    pthread_t t1;
    int err;
    int status;

    init_data *data = malloc(sizeof(init_data));
    data->tid = &t1;
    data->posizione = 0;
    
    err = pthread_create(&t1, NULL, &turista, (void *) data);
    
    if (err != 0)
    {
        perror("Creating T1");
        exit(EXIT_FAILURE);
    } 

    sleep(5);

    pthread_cond_broadcast(&stazione);

    pthread_join(t1, (void *) &status);
    printf("T1: %d\n", status);
    */
    return 0;
}
