#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "utils.h"


// inizializza il mutex per la cabina
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; 

// CV per centro e stazione
static pthread_cond_t stazione= PTHREAD_COND_INITIALIZER;
static pthread_cond_t centro_storico = PTHREAD_COND_INITIALIZER;

static int cond = 1;
static const int POSTI_MAX = 4;
static int passeggeri = 0;

static void *cabina(void *arg)
{
    int posizione = 0;
    int *myid = (int *) arg;
    while(1)
    {
        printf("In viaggio...\n");
        sleep(2);
        printf("Arrivo\n");
    }

    return((void *) 0);
}

static void *turista(void *arg)
{
    init_data *data = (init_data *) arg;
    pthread_t *id = data->tid;
    int posizione = data->posizione;

    if (posizione == 0)
    {
        thread_cond_wait(&stazione, &mtx);
    } else
    {
        thread_cond_wait(&centro_storico, &mtx);
    }
    
    printf("ID: %ld, posizione: %d\n", *id, posizione);

    return((void *) 0);
}



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
    
    return 0;
}
