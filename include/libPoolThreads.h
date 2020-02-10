/**
*Fuente de la libreria de getion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool y una función de prueba para los hilos.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#ifndef POOL_THREADS
#define POOL_THREADS

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10 /*Cantidad de threads a crear en la pool*/

typedef struct{
    pthread_t tid[NUM_THREADS];
    int num_threads;
    int *num_working;
    bool *stop;
    pthread_mutex_t shared_mutex;
    void *function;
}pool_thread;

typedef struct{
    int *num_working;
    bool *stop;
    pthread_mutex_t shared_mutex;
    void *function;
}thread_args;
 
/****
*FUNCIÓN: pool_thread  *pool_th_ini()
*ARGS_IN: none
*DESCRIPCION: Crea un pool de threads con una cantidad de threads definida
*en la macro NUM_THREADS.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread *pool_th_ini();

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir.
          int numThreads: Cuantos threads tiene la pool.
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threads, int numThreads);

/****
*FUNCIÓN: Funcion de prueba para los hilos.
*ARGS_IN: void* args: Se le pasa un puntero a una estructura thread_args
*que contiene argumentos de control del hiloargumentos de control del hilo.
*DESCRIPCION: Funcion que prueba que los hilos y los argumentos
*pasados a ellos funcionan correctamente.
*ARGS_OUT: none
****/
void th_main(void *args);

#endif 
