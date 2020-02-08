/**
*Fuente de la libreria de getion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include <pthread.h>
 
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
void pool_th_destroy(pool_thread p_threads, int numThreads){
