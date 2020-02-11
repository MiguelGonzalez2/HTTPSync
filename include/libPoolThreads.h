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
#include <stdio.h>
#include <stdlib.h>

typedef struct _pool_thread pool_thread;

/****
*FUNCIÓN: pool_thread  *pool_th_ini()
*ARGS_IN: none
*DESCRIPCION: Crea un pool de threads con una cantidad de threads indicada
*por la variable num_threads.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread *pool_th_ini();

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir.
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threadss);

/****
*FUNCIÓN: void pool_th_stop(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads que paren de procear.
*ARGS_OUT: none
****/
void pool_th_stop(pool_thread *p_threads);

/****
*FUNCIÓN: void pool_th_wait(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads de una pool que se paren.
*ARGS_OUT: none
****/
void pool_th_wait(pool_thread *p_threads);

#endif 
