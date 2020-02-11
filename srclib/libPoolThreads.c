/**
*Fuente de la libreria de getion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool y una función de prueba para los hilos.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include "../include/libPoolThreads.h"

struct _pool_thread{
    pthread_t *tid;
    int num_threads;
    int *num_working;
    int *stop;
    pthread_mutex_t shared_mutex;
    void *function;
};
 
typedef struct{
    int *num_working;
    int *stop;
    pthread_mutex_t shared_mutex;
    void *function;
}thread_args;

void th_main(void *args);

/****
*FUNCIÓN: pool_thread  *pool_th_ini()
*ARGS_IN: func - Puntero a la funcion que procesan los hilos.
          int num_threads - Numero de threads a crear.
*DESCRIPCION: Crea un pool de threads con una cantidad de threads indicada
*por la variable num_threads.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread  *pool_th_ini(void *func, int num_threads){

    pool_thread *p_threads;
    thread_args *args;
    int i;
    int error = 0;

    if(num_threads <= 0) return NULL;

    p_threads = malloc(sizeof(pool_thread));
    p_threads->stop = malloc(sizeof(int));
    p_threads->num_working = malloc(sizeof(int));
    p_threads->tid = malloc(sizeof(pthread_t) * num_threads);

    args = malloc(sizeof(args));
    args->stop = malloc(sizeof(int));
    args->num_working = malloc(sizeof(int));

    p_threads->num_threads = 0;
    *p_threads->num_working = 0;
    *p_threads->stop = 0;

    if(pthread_mutex_init(&p_threads->shared_mutex, NULL) != 0){
        free(p_threads);
        fprintf(stderr,"No se pudo iniciar el semáforo");
        return NULL;
    }
    
    args->shared_mutex = p_threads->shared_mutex;
    args->function = func;
    args->stop = p_threads->stop;
    args->num_working = p_threads->num_working;

    for(i = 0; i < num_threads; i++){
        if(pthread_create(&p_threads->tid[i], NULL, (void *)&th_main, args) !=0){
            fprintf(stderr,"No se pudo iniciar uno de los threads");
            error = 1;
            break;
        }
        p_threads->num_threads++;
    }
    if(error != 0){

        for(i = 0; i < p_threads->num_threads; i++){
            pthread_cancel(p_threads->tid[i]);
        }

        pool_th_destroy(p_threads);
        return NULL;
    }

    return p_threads;

}

/****
*FUNCIÓN: Funcion de prueba para los hilos.
*ARGS_IN: void* args: Se le pasa un puntero a una estructura thread_args
*que contiene argumentos de control del hiloargumentos de control del hilo.
*DESCRIPCION: Funcion que prueba que los hilos y los argumentos
*pasados a ellos funcionan correctamente.
*ARGS_OUT: none
****/
void th_main(void *args){

    thread_args *argumentos = args;

    while(!(*argumentos->stop)){
    if(!(*argumentos->stop)){
        pthread_mutex_lock(&argumentos->shared_mutex);
        (*argumentos->num_working)++;
        pthread_mutex_unlock(&argumentos->shared_mutex);  

        printf("Vivan las redes de comunicacion\n");
        sleep(1);

        pthread_mutex_lock(&argumentos->shared_mutex);
        (*argumentos->num_working)--;
        pthread_mutex_unlock(&argumentos->shared_mutex);  
    }
    }
    
    pthread_exit(EXIT_SUCCESS);
}

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir. 
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threads){
    int i;

    pthread_mutex_destroy(&p_threads->shared_mutex); 
    free(p_threads->num_working);
    free(p_threads->stop);
    free(p_threads->tid);
    free(p_threads);
}

/****
*FUNCIÓN: void pool_th_stop(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads que paren de procear.
*ARGS_OUT: none
****/
void pool_th_stop(pool_thread *p_threads){

    *p_threads->stop = 1;
}

/****
*FUNCIÓN: void pool_th_wait(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads de una pool que se paren.
*ARGS_OUT: none
****/
void pool_th_wait(pool_thread *p_threads){
    int i;

    for(i = 0; i < p_threads->num_threads; i++){
        pthread_join(p_threads->tid[i], NULL);
    }
}
