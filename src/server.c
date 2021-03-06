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
#include "http_request.h"
#include "http_reply.h"
#include "configFile.h"
 

int end = 0; /*Indica el fin del programa*/
config_t *config = NULL; /*Configuraciones*/

/*Manejador para señal*/
void end_handler(int sig){
    end = 1;  
}

/****
*FUNCIÓN: int thread_work()
*ARGS_IN: int server_fd: Descriptor del server.
*DESCRIPCION: Funcion que llevara a cabo cada hilo.
*ARGS_OUT: 0 si exito, -1 si fallo.
****/
int thread_work(int server_fd){

   int conn_fd, port, addr, status;
   request_t *req;
   connectionStatus cstatus = Open;
   connectionStatus replyStatus;

   syslog(LOG_INFO, "ServerHTTP: Hilo esperando conexiones.\n");
   pool_th_cancel_state(0);
   conn_fd = socket_accept(server_fd, &port, &addr);
   pool_th_cancel_state(1);
   if(conn_fd == -1){
      /*Solo consideramos que ha sido fallo si no fue por EINTR*/
      return (errno == EINTR);
   }

   syslog(LOG_INFO, "ServerHTTP: Conexion establecida en puerto %d e IP %d\n", port, addr);

   /*Ajustamos un timer a las operaciones de lectura para evitar stalls del cliente*/
   status = socket_set_read_timer(conn_fd, get_config_file_readTimeout(config));
   if(status == -1){
       syslog(LOG_ERR, "ServerHTTP: Error estableciendo timer del socket\n");
       return -1;
   } 
   while(cstatus == Open && !end){ 
        req = http_get_request(conn_fd, get_config_file_serverRoot(config)); 
        if(req != NULL){ 
            syslog(LOG_INFO,"ServerHTTP: Request caught. Method %s. Path %s. Errorcode %d\n",     http_get_method(req),http_get_path(req),http_get_error(req));
            syslog(LOG_INFO,"ServerHTTP: Sending reply\n");
            cstatus = http_get_connection_status(req);
            replyStatus = (end) ? Close : Open;
	    status = http_reply_send(conn_fd, req, replyStatus, get_config_file_serverSignature(config));
            syslog(LOG_INFO,"ServerHTTP: %d Bytes of Reply Sent\n", status);
            http_req_destroy(req);
        } else {
            /*Salta el timer*/
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                syslog(LOG_INFO, "ServerHTTP: Closing socket on client timeout.");
                /*Enviamos un 408 Request Timeout*/
                cstatus = TimedOut;
                http_reply_send(conn_fd, NULL, cstatus, get_config_file_serverSignature(config));
            }
            if(!end && errno != EINTR){
                syslog(LOG_ERR,"ServerHTTP: Error getting request\n");
                cstatus = Close;
            }
        }
   }
 
  socket_close(conn_fd);  
  syslog(LOG_INFO, "ServerHTTP: Cerrando la conexion en puerto %d e IP %d\n", port, addr);
  return 0; 
}

/****
*FUNCIÓN: int end_handler_signal(int signum)
*ARGS_IN: int signum: Señal que marcara el fin del programa.
*DESCRIPCION: Permite sobreescribir la accion de una señal por un nop.
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
*FUNCIÓN: void initialize_mask(int signal)
*ARGS_IN: int signal: Senal a enmascarar
*         int *ret: Se pondra a 0 si fue bien o -1 si fallo.
*ARGS_OUT: sigset_t: Mascara antigua, o mascara vacia si falla.
*DESCRIPCION: Inicializa una mascara con la señal pasada, o vacia si
* se pasa un -1.
****/
sigset_t initialize_mask(int signal, int *ret){
    sigset_t set;
    sigset_t old;
    int status;
    
    sigemptyset(&set);
    if(signal != -1){
        sigaddset(&set, signal);
    }

    status = pthread_sigmask(SIG_SETMASK, &set, &old);

    if(ret != NULL){
        *ret = status;
    }

    if(status == 0){
        return old;
    }
    
    sigemptyset(&old);
    return old;
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
   int status;
   sigset_t old;
   pool_thread *pool;

   /*Parseo del fichero de configuracion*/
   config = ini_config_file();
   if(config == NULL){
       syslog(LOG_ERR, "Error al parsear el fichero de configuracion");
       return EXIT_FAILURE;
   }

   /*DAEMON*/
   if(get_config_file_daemonMode(config)){ 
       if(daemonProcess() != EXIT_SUCCESS){
           syslog(LOG_ERR, "ServerHTTP: Error creando daemon.\n");
           free_config_file(config);
           return EXIT_FAILURE;
       }
   }
   
   /*Ignoramos SIGINT, es la que cerrara de manera limpia el server*/
   if(end_handler_signal(SIGINT) != 0){
       syslog(LOG_ERR, "ServerHTTP: Error creando el manejador de la senial SIGINT.\n");
       return EXIT_FAILURE;
   }

   /*Enmascaramos SIGINT*/
   old = initialize_mask(SIGINT, &status);
   if(status == -1){
       syslog(LOG_ERR, "ServerHTTP: Error enmascarando senales.\n");
       free_config_file(config);
       return EXIT_FAILURE;
   }

   /*Abrimos el servidor*/
   server_fd = socket_server_init(NULL, get_config_file_port(config), get_config_file_listenQueue(config));
   if(server_fd == -1){
       syslog(LOG_ERR, "ServerHTTP: Error inicializando server.\n");
       free_config_file(config);
       return EXIT_FAILURE;
   }

   /*Inicializamos el trabajo*/
   pool = pool_th_ini(&thread_work, get_config_file_maxClients(config), server_fd);
   if(pool == NULL){
       syslog(LOG_ERR, "ServerHTTP: Error inicializando pool de hilos.\n");
       free_config_file(config);
       socket_close(server_fd);
       return EXIT_FAILURE;
   }

   syslog(LOG_INFO, "ServerHTTP: Servidor operativo.\n");

   /*A la espera de una señal de cierre*/
   sigsuspend(&old);

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

   /*Liberamos la struct con el config file parseado*/
   free_config_file(config);

   syslog(LOG_INFO, "ServerHTTP: Cierre del servidor con exito\n");
   
   exit(EXIT_SUCCESS);
}
