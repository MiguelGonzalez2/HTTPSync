/**
*Fuente de la libreria de getion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool y una función de prueba para los hilos.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include "../include/pool_threads.h"

/****
*FUNCIÓN: pool_thread  *pool_th_ini()
*ARGS_IN: func - Puntero a la funcion que procesan los hilos.
*DESCRIPCION: Crea un pool de threads con una cantidad de threads definida
*en la macro NUM_THREADS.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread  *pool_th_ini(void *func){

    pool_thread *p_threads;
    thread_args *args;
    int i;
    bool error = false;

    if(NUM_THREADS <= 0) return NULL;

    p_threads = malloc(sizeof(p_threads) * NUM_THREADS);
    p_threads->stop = malloc(sizeof(bool));
    p_threads->num_working = malloc(sizeof(int));

    args = malloc(sizeof(args));
    args->stop = malloc(sizeof(bool));
    args->num_working = malloc(sizeof(int));

    p_threads->num_threads = NUM_THREADS;
    *p_threads->num_working = 0;
    *p_threads->stop = false;

    if(pthread_mutex_init(&p_threads->shared_mutex, NULL) != 0){
        free(p_threads);
        fprintf(stderr,"No se pudo iniciar el semáforo");
        return NULL;
    }
    
    args->shared_mutex = p_threads->shared_mutex;
    args->function = func;
    args->stop = p_threads->stop;
    args->num_working = p_threads->num_working;

    for(i = 0; i < NUM_THREADS; i++){
        if(pthread_create(&p_threads->tid[i], NULL, (void *)&th_main, args) !=0){
            fprintf(stderr,"No se pudo iniciar uno de los threads");
            error = true;
            break;
        }
    }
    if(error == true){
        pool_th_destroy(p_threads,i);
        return NULL;
    }

    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(p_threads->tid[i], NULL);
    }

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
    
    if(!(*argumentos->stop)){
        pthread_mutex_lock(&argumentos->shared_mutex);
        (*argumentos->num_working)++;
        pthread_mutex_unlock(&argumentos->shared_mutex);  

        printf("Vivan las redes de comunicacion\n");

        pthread_mutex_lock(&argumentos->shared_mutex);
        (*argumentos->num_working)--;
        pthread_mutex_unlock(&argumentos->shared_mutex);  
    }
    
    pthread_exit(EXIT_SUCCESS);
}

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir. 
          int numThreads: Cuantos threads tiene la pool.
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threads, int numThreads){
    int i;
    for(i = 0; i < numThreads; i++){
        pthread_cancel(p_threads->tid[i]);
    }
    pthread_mutex_destroy(&p_threads->shared_mutex); 
    free(p_threads->num_working);
    free(p_threads->stop);
    free(p_threads);

}
