#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Struttura utilizzata per passare i dati di inizializzazione ai thread Turista
struct init_data
{
    pthread_t *tid;
    int posizione;
};
typedef struct init_data init_data;


void thread_create(pthread_t *id, const pthread_attr_t *attr, void *(*function)(void *), void *arg);
void thread_join(pthread_t id, void **status);

void thread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);
void thread_cond_broadcast(pthread_cond_t *cond);

void thread_mutex_lock(pthread_mutex_t *mutex);
void thread_mutex_unlock(pthread_mutex_t *mutex);
