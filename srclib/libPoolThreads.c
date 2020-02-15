/**
*Fuente de la libreria de gestion de la pool de hilos. Proporciona una interfaz
*para crear la pool, destruir la pool y una función de prueba para los hilos.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include "libPoolThreads.h"
 
struct _thread_args{
    int num_working;
    int stop;
    int server_fd;
    pthread_mutex_t *shared_mutex;
    task_function function;
};

struct _pool_thread{
    pthread_t *tid;
    int num_threads;
    struct _thread_args *args;
};

void th_main(struct _thread_args *argumentos);

/****
*FUNCIÓN: pool_thread  *pool_th_ini(void *func, int num_threads)
*ARGS_IN: task_function func: Funcion que ejecutara cada hilo.
          int num_threads: numero de hilos a correr.
          int server_fd: Descriptor del server que opera los hilos.
*DESCRIPCION: Crea un pool de threads con una cantidad de threads indicada
*por la variable num_threads.
*ARGS_OUT: pool_thread* - Devuelve la pool de threads, o NULL si hay error.
****/
pool_thread *pool_th_ini(task_function func, int num_threads, int server_fd){

    pool_thread *p_threads;
    int i;
    int error = 0;

    if(num_threads <= 0) return NULL;

    /*Reservas para la pool de hilos*/
    p_threads = malloc(sizeof(pool_thread));
    if(p_threads == NULL){
        return NULL;
    }

    p_threads->args = malloc(sizeof(struct _thread_args));
    if(p_threads->args == NULL){
        free(p_threads);
        return NULL;
    }

    p_threads->tid = malloc(sizeof(pthread_t) * num_threads);
    p_threads->args->shared_mutex = malloc(sizeof(pthread_mutex_t));

    if(p_threads->tid == NULL || p_threads->args->shared_mutex == NULL){
        error = 1;
    }
    
    /*Inicializamos valores*/
    if(error == 0){

	    p_threads->num_threads = 0;
	    p_threads->args->num_working = 0;
	    p_threads->args->stop = 0;
            p_threads->args->function = func;
            p_threads->args->server_fd = server_fd;

            if(pthread_mutex_init(p_threads->args->shared_mutex, NULL) != 0){
                error = 1;
            }

            for(i = 0; i < num_threads; i++){
                if(pthread_create(&p_threads->tid[i], NULL, (void * (*)(void *))&th_main, p_threads->args) != 0){
                    syslog(LOG_ERR, "No se pudo iniciar uno de los threads");
                    error = 1;
                    break;
                }
                p_threads->num_threads++;
             }
    }
   
    /*Control de errores*/
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
*ARGS_IN: void* argumentos: Se le pasa un puntero a una estructura thread_args
*que contiene argumentos de control del hiloargumentos de control del hilo.
*DESCRIPCION: Funcion que prueba que los hilos y los argumentos
*pasados a ellos funcionan correctamente.
*ARGS_OUT: none
****/
void th_main(struct _thread_args *argumentos){

    int status;

    if(argumentos == NULL){
        return;
    }

    while(!(argumentos->stop)){
    	if(!(argumentos->stop)){
        	pthread_mutex_lock(argumentos->shared_mutex);
        	(argumentos->num_working)++;
        	pthread_mutex_unlock(argumentos->shared_mutex);  

        	/*Llamada a la funcion de trabajo*/
        	status = argumentos->function(argumentos->server_fd);
        	if(status != 0){
            		pthread_exit(NULL);
        	}

        	pthread_mutex_lock(argumentos->shared_mutex);
        	(argumentos->num_working)--;
        	pthread_mutex_unlock(argumentos->shared_mutex);  
    	}
    }
    
    pthread_exit(NULL);
}

/****
*FUNCIÓN: void pool_th_destroy(pool_thread p_threads, int numThreads)
*ARGS_IN: pool_thread p_threads: La pool de threads a destruir. 
*DESCRIPCION: Encargada de liberar los datos reservados para la pool de threads.
*ARGS_OUT: none
****/
void pool_th_destroy(pool_thread *p_threads){
    
    if(p_threads == NULL){
        return;
    }
    
    if(p_threads->args != NULL){
        if(p_threads->args->shared_mutex != NULL){
            free(p_threads->args->shared_mutex);
        }

        free(p_threads->args);
    }
    
    if(p_threads->tid != NULL){
        free(p_threads->tid);
    }

    free(p_threads);
}

/****
*FUNCIÓN: void pool_th_stop(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads que paren de procear.
*ARGS_OUT: none
****/
void pool_th_stop(pool_thread *p_threads){
    int i;

    if(p_threads == NULL){
        return;
    }

    p_threads->args->stop = 1;

    /*Sacar a los threads de bloqueos*/
    for(i = 0; i < p_threads->num_threads; i++){
        pthread_kill(p_threads->tid[i], SIGINT);
    }
}

/****
*FUNCIÓN: void pool_th_wait(pool_thread *p_threads)
*ARGS_IN: pool_thread p_threads: La pool de threads a parar.
*DESCRIPCION: Encargada de indicar a los threads de una pool que se paren.
*ARGS_OUT: none
****/
void pool_th_wait(pool_thread *p_threads){
    int i;

    if(p_threads == NULL){
        return;
    }

    for(i = 0; i < p_threads->num_threads; i++){
        pthread_join(p_threads->tid[i], NULL);
    }
}
