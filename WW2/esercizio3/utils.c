#include "utils.h"

/* Gestisce gli errori della funzione pthread_cond_wait */
void thread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex)
{
    int err = pthread_cond_wait(cv, mutex);

    if (err != 0)
    {
        perror("Waiting");
        exit(EXIT_FAILURE);
    }

    return;
}