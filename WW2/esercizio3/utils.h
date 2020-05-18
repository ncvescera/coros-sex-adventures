#if !defined(UTILS_H_)
#define UTILS_H_
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <pthread.h>

    enum fermata {STAZIONE, CENTRO};

    // Struttura utilizzata per passare i dati di inizializzazione ai thread Turista
    struct init_data
    {
        pthread_t *tid;
        enum fermata posizione;
    };

    typedef struct init_data init_data; // typedef per semplificare il nome di struct init_data
    typedef enum fermata fermata;

    void thread_create(pthread_t *id, const pthread_attr_t *attr, void *(*function)(void *), void *arg);    /* Gestisce gli errori della funzione pthread_create */
    void thread_join(pthread_t id, void **status);  /* Gestisce gli errori della funzione pthread_join */

    void thread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mutex);  /* Gestisce gli errori della funzione pthread_cond_wait */
    void thread_cond_broadcast(pthread_cond_t *cond);   /* Gestisce gli errori della funzione pthread_cond_broadcast */

    void thread_mutex_lock(pthread_mutex_t *mutex); /* Gestisce gli errori della funzione  pthread_mutex_lock */
    void thread_mutex_unlock(pthread_mutex_t *mutex);   /* Gestisce gli errori della funzione  pthread_mutex_unlock */

    char *post_to_str(fermata pos);

#endif