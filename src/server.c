/**
*Fichero con el main del servidor.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include "daemon.h"
#include "libPoolThreads.h"
#include "libsocket.h"
 
#define HTTP_SERVER_PORT 8080 /*!<Puerto para el server*/
#define LISTEN_QUEUE 5 /*!<Cola de espera para conexiones no aceptadas*/
#define THREAD_NO 20 /*!<Numero de hilos*/

/*Manejador para señal*/
void end_handler(int sig){  
}

/****
*FUNCIÓN: int thread_work()
*ARGS_IN: int server_fd: Descriptor del server.
*DESCRIPCION: Funcion que llevara a cabo cada hilo.
*ARGS_OUT: 0 si exito, -1 si fallo.
****/
int thread_work(int server_fd){

   int conn_fd, port, addr;
   /*DECLARACIONES DEL CODIGO TEMPORAL*/
   char buffer[8192] = "HTTP/1.1 200 OK\r\nContent-Length: 44\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body><h1>It works!</h1></body></html>\r\n";
   /*FIN DECLARACIONES DEL CODIGO TEMPORAL*/

   syslog(LOG_INFO, "ServerHTTP: Hilo esperando conexiones.\n");
   conn_fd = socket_accept(server_fd, &port, &addr);
   if(conn_fd == -1){
      /*Solo consideramos que ha sido fallo si no fue por EINTR*/
      return (errno == EINTR);
   }

   syslog(LOG_INFO, "ServerHTTP: Conexion establecida en puerto %d e IP %d\n", port, addr);

   /*TODO: CAMBIAR ESTA PIEZA DE CODIGO POR UNA LLAMADA A LIBRERIA HTTP*/
   /*PARA PROBAR; SE MANDARA UN FICHERO PREDEFINIDO*/
   if(socket_send(conn_fd, buffer, strlen(buffer)) == -1){
	socket_close(conn_fd);
	return -1;
				}
  /*FIN DEL CODIGO TEMPORAL*/

  socket_close(conn_fd);  
  syslog(LOG_INFO, "ServerHTTP: Cerrando la conexion en puerto %d e IP %d\n", port, addr);
  return 0; 
}

/****
*FUNCIÓN: int end_handler_signal(int signum)
*ARGS_IN: int signum: Señal que marcara el fin del programa.
*DESCRIPCION: Permite sobreescribir la accicon de una señal por un nop.
*ARGS_OUT: int - 0 si exito, -1 si fallo.
****/
int end_handler_signal(int signum){
    struct sigaction sa;

    sa.sa_handler = &end_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    return sigaction(signum, &sa, NULL);
}

/****
*FUNCIÓN: int main(int argc, char **argv)
*ARGS_IN: int argc: Numero de parametros de entrada
          char **argv: Parametros de entrada
*DESCRIPCION: Inicializa un server en modo demonio que escucha y responde
*mediante HTTP hasta la recepcion de una senal SIGINT.
*ARGS_OUT: int - EXIT_SUCCESS si hay exito.
****/
int main(int argc, char **argv){
   int server_fd;
   pool_thread *pool;

   /*DAEMON*/ 
   if(daemonProcess() != EXIT_SUCCESS){
       syslog(LOG_ERR, "ServerHTTP: Error creando daemon.\n");
       return EXIT_FAILURE;
   }
   
   /*Ignoramos SIGINT, es la que cerrara de manera limpia el server*/
   if(end_handler_signal(SIGINT) != 0){
       syslog(LOG_ERR, "ServerHTTP: Error creando el manejador de la senial SIGINT.\n");
       return EXIT_FAILURE;
   }

   /*Abrimos el servidor*/
   server_fd = socket_server_init(NULL, HTTP_SERVER_PORT, LISTEN_QUEUE);
   if(server_fd == -1){
       syslog(LOG_ERR, "ServerHTTP: Error inicializando server.\n");
       return EXIT_FAILURE;
   }

   /*Inicializamos el trabajo*/
   pool = pool_th_ini(&thread_work, THREAD_NO, server_fd);
   if(pool == NULL){
       syslog(LOG_ERR, "ServerHTTP: Error inicializando pool de hilos.\n");
       socket_close(server_fd);
       return EXIT_FAILURE;
   }

   /*A la espera de una señal de cierre*/
   pause();

   syslog(LOG_INFO, "ServerHTTP: Inicio de cierre del servidor.\n");

   /*Decimos a los hilos que paren*/
   pool_th_stop(pool);
   /*Esperamos a que acaben*/
   pool_th_wait(pool);
   syslog(LOG_INFO, "ServerHTTP: Servidor esperando a que los threads paren.\n");

   /*Liberamos la pool*/
   pool_th_destroy(pool);
   /*Cerramos el socket*/ 
   socket_close(server_fd);

   syslog(LOG_INFO, "ServerHTTP: Cierre del servidor con exito\n");
   
   exit(EXIT_SUCCESS);
}
