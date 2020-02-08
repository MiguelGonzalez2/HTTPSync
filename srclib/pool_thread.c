/**
*Fuente de la libreria de getion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include <poolThreads.h>

/****
*FUNCIÓN: pool_thread  *pool_th_ini()
*ARGS_IN: none
*DESCRIPCION: Crea un pool de threads con una cantidad de threads definida
*en la macro NUM_THREADS.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread  *pool_th_ini(){

    pool_thread *p_threads;
    int i;
    bool error = false;

    if(num_threads <= 0) return NULL;

    p_threads = (pool_thread) malloc(sizeof(*p_threads));

    p_threads.num_threads = NUM_THREADS;
    p_threads.num_working = 0;
    p_threads.stop = false;

    if(!pthread_mutex_init(p_threads->shared_mutex)){
        free(p_threads);
        return NULL;
    }

    for(i = 0; i < NUM_THREADS; i++){
        if(pthread_create(p_threads->tid[i]) != 0){
            error = true;
            break;
        }
    }
    if(error == true){
        pool_thread_destroy(p_threads,i);
        return NULL;
    }
}

/****
*FUNCIÓN:
*ARGS_IN:
*DESCRIPCION: 
*ARGS_OUT:
****/
void th_main(){

}

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir. 
          int numThreads: Cuantos threads tiene la pool.
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread p_threads, int numThreads){
    int i;
    for(i = 0; i < numThreads; i++){
        pthread_cancel(p_threads->tid[i]);
    }
    pthread_mutex_destroy(p_threads->pthread_mutex_t); 
    free(p_threads);

}
