#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct init_data
{
    pthread_t *tid;
    int posizione;
};
typedef struct init_data init_data;

void thread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);
