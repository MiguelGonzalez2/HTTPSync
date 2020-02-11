/**
*Fuente de la libreria de gestion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool y una función de prueba para los hilos.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#ifndef POOL_THREADS
#define POOL_THREADS

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _pool_thread pool_thread; /*!< Pool de hilos*/
typedef void (*task_function)(); /*!< Funcion que ejecutara cada hilo*/

/****
*FUNCIÓN: pool_thread  *pool_th_ini(void *func, int num_threads)
*ARGS_IN: task_function func: Funcion que ejecutara cada hilo.
          int num_threads: numero de hilos a correr.
*DESCRIPCION: Crea un pool de threads con una cantidad de threads indicada
*por la variable num_threads.
*ARGS_OUT: pool_thread - Devuelve la pool de threads.
****/
pool_thread *pool_th_ini(task_function func, int num_threads);

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir.
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threads);

/****
*FUNCIÓN: void pool_th_stop(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads que paren de procesar.
*ARGS_OUT: none
****/
void pool_th_stop(pool_thread *p_threads);

/****
*FUNCIÓN: void pool_th_wait(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a esperar.
*DESCRIPCION: La funcion se bloquea hasta que hayan finalizado su ejecucion todos los hilos.
*ARGS_OUT: none
****/
void pool_th_wait(pool_thread *p_threads);

#endif 
