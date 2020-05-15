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
static pthread_cond_t in_mini = PTHREAD_COND_INITIALIZER;

// Posti del minimetro
static const int POSTI_MAX = 4; // numero massimo di passeggeri per navetta
static int n_posti; 

static void *cabina(void *arg)
{
    //int *myid = (int *) arg; // qualora servisse l'id del thread
    n_posti = POSTI_MAX; // inizializza il numero di posti dipsonibili
    int posizione = 0;  // la posizione attuale del mini, 0 stazione, 1 centro storico

    while (1)
    {
        // identifica quali thread svegliare
        pthread_cond_t *local_posizione;

        if (posizione == 0)
        {
            local_posizione = &stazione;
        } else
        {
            local_posizione = &centro_storico;
        }

        // sveglia i thread della stazione in cui sta il mini
        thread_cond_broadcast(local_posizione);

        // aspetta che tutti i passeggeri siano a bordo
        //printf("Aspetto\n");
        //fflush(stdout);
        while (n_posti > 0)
        {
            sleep(1);
            continue;
        }

        printf("Posti liberi: %d\n", n_posti);

        // blocca la risorsa dagli altri thread
        thread_mutex_lock(&mtx);

        printf("\nPARTENZA ...\n");
        sleep(2);
        printf("Arrivo!\n\n");

        // libera la risorsa
        thread_mutex_unlock(&mtx);

        // ripristina il numero di posti liberi (fa scendere i passeggeri e libera i posti)
        thread_cond_broadcast(&in_mini);

        while (n_posti < POSTI_MAX)
        {
            sleep(1);
            continue;
        }

        printf("\n");

        // inversione della stazione
        if (posizione == 0)
        {
            posizione = 1;
        } else
        {   
            posizione = 0;
        }

        sleep(2);   // attende 2 secondi altrimenti i turisti non riceverebbero il signale per svegliarsi
    }

    return((void *) 0);
}

static void *turista(void *arg)
{
    // inizializza il turista
    init_data *data = (init_data *) arg;
    pthread_t *id = data->tid;
    int posizione = data->posizione;

    // elimina la memoria allocata dinamicamente ormai non più utile
    free(data);

    while (1)
    {
        // identifica la stazione in cui si trova
        pthread_cond_t *local_posizione;

        if (posizione == 0)
        {
            local_posizione = &stazione;
            
        } else
        {   
            local_posizione = &centro_storico;
        }

        // aspetta che il mini gli dia la possibilità di salire
        thread_cond_wait(local_posizione, &mtx);

        // controlla se ci sono posti liberi
        if (n_posti)
        {
            // prende posto nel minimetro
            n_posti --;

            printf("ID: %ld sale\n", *id);

            // libera la risorsa
            thread_mutex_unlock(&mtx);

            // aspetta di poter scendere e liberare il posto
            thread_cond_wait(&in_mini, &mtx);
            
            n_posti ++;

            printf("ID: %ld scende\n", *id);
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

        } else  // se non ci sono posti liberi rilascia la risorsa (acquisita dopo essersi svegliato) e ricomincia
        {
            thread_mutex_unlock(&mtx);
        }
    }

    return((void *) 0);
}

// Il numero di turisti da inizializzare
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
    pthread_t turisti[NTURISTI];

    // inizializzo un thread con posizione centro storico
    init_data *data = malloc(sizeof(init_data));
    data->tid = &turisti[0];
    data->posizione = 1;

    thread_create(&turisti[0], NULL, &turista, (void *) data);

    printf("ID: %ld ", turisti[0]);

    // inizializzo gli altri thread con posizione stazione
    for (int i = 1; i < NTURISTI; i++)
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
