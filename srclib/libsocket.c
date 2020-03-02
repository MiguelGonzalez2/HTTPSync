/**
* Fuente de la libreria de gestion de sockets TCP. Proporciona una interfaz
* comoda para abrir un socket TCP en modo escucha y gestionar
* conexiones.
*
* @author Miguel Gonzalez, Alejandro Bravo
* @version 1.0b
* @date 06-02-2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> /*Socket llamadas de sistema*/
#include <arpa/inet.h> /*AF_INET tipo de socket*/
#include <string.h> /*Memset*/
#include <netinet/in.h> /*struct sockaddr_in*/
#include <sys/time.h> /*Timer*/
#include <fcntl.h>
#include <unistd.h> 
#include "libsocket.h"

/****
* FUNCIÓN: int socket_server_init(char *ip, int port, int listen_size)
* ARGS_IN: char *ip: Direccion IP de la interfaz por la que escucha el server. 
*                    Puede asignarse a NULL para que escuche por todas.
*          int port: Numero de puerto en el que escuchara el servidor.
*          int listen_size: Tamano de la cola de escucha. Como maximo puede
*                           haber listen_size peticiones a la espera de establecer
*                           la conexion TCP.
* DESCRIPCIÓN: Crea un socket para el servidor TCP escuchando en la IP y puerto asignadas.
* ARGS_OUT: int - Devuelve el identificador (descriptor de fichero) del socket abierto, o -1 en
*                 caso de error.
****/
int socket_server_init(char *ip, int port, int listen_size){
	
	int socket_fd;
	struct sockaddr_in addr;
	
	/*Comprobacion de errores*/
	if(port < 1 || listen_size <= 0) return -1;
	
	/*Apertura de socket TCP*/
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) return -1;
	
	/*Preparamos la estructura direccion/puerto*/
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	if(ip == NULL){
		addr.sin_addr.s_addr = INADDR_ANY;
	} else {
		addr.sin_addr.s_addr = inet_addr(ip);
	}
	addr.sin_port = htons(port);
	
	/*Enlazamos el socket a esa direccion/puerto*/
	if(bind(socket_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0){
		close(socket_fd);
	    return -1;	
	}
	
	/*Escuchamos*/
	if(listen(socket_fd,listen_size) != 0){
	    close(socket_fd);
		return -1;
	}
	
	return socket_fd;
	
}

/****
* FUNCIÓN: int socket_close(int socketfd)
* ARGS_IN: int fd - Descriptor del socket o conexion a cerrar.
* DESCRIPCIÓN: Cierra un socket o una conexion, liberando los recursos asociados.
* ARGS_OUT: int - Devuelve 0 si se ha cerrado correctamente, o -1 en caso contrario.
****/
int socket_close(int fd){
    return close(fd);
}

/****
* FUNCIÓN: int socket_accept(int socket_fd)
* ARGS_IN: int socket_fd: Descriptor del servidor del que se acepta la conexion.
*          int *port: Direccion de memoria donde se escribira el puerto del cliente. En orden usual (no de red).
*          int *addr: Direccion de memoria donde se escribira el valor de la IP origen. En orden usual (no de red).
* DESCRIPCIÓN: Establece la conexion con un cliente. Es bloqueante hasta que se reciba una peticion de conexion.
* ARGS_OUT: int - Devuelve el identificador (descriptor) de la conexion con el cliente, o -1 en caso de error.
****/
int socket_accept(int socket_fd, int *port, int *addr){
	
	int conn_fd;
	struct sockaddr_in saddr;
	socklen_t len = sizeof(struct sockaddr_in);
	
	if(port == NULL || addr == NULL) return -1;
	
	conn_fd = accept(socket_fd, (struct sockaddr *) &saddr, &len);
	if(conn_fd == -1){
	   return -1;
	}
	
	/*Extraemos las direcciones de la conexion entrante*/
	*port = ntohs(saddr.sin_port);
	*addr = ntohl(saddr.sin_addr.s_addr);
	
	return conn_fd;
}

/****
* FUNCIÓN: int socket_send(int socket_fd char *data, int size)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion a la que enviar los datos.
*          char* data - comienzo de los bytes que se desean transferir.
*          int size: Numero de bytes a transferir.
* DESCRIPCIÓN: Envia datos a traves de un socket/conexion.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int socket_send(int socket_fd, char *data, int size){
	
	int i, num_chunks, nbytes, ret, written = 0;
	
	if(data == NULL || size <= 0){
	    return -1;
	}
	
	/*Calculamos el numero de trozos*/
	num_chunks = (size-1) / CHUNK_SIZE +1;
	
	/*Los vamos enviando sucesivamente*/
	for(i = 0; i < num_chunks; i++){
		/*Calculamos cuantos bytes van en este chunk*/
		if(i == (num_chunks-1)){
		    nbytes = size - CHUNK_SIZE*i;	
		} else {
			nbytes = CHUNK_SIZE;
		}
		
		/*Escribimos*/
		ret = write(socket_fd, data + i*CHUNK_SIZE, nbytes); 
		if(ret == -1){
			return written;
		}
		
		written += ret;
	}
	
	return written;
	
}

/****
* FUNCIÓN: int socket_receive(int socket_fd, char *data, int size)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion por el que recibir los datos.
*          char* data - comienzo del espacio de memoria donde se copiaran los datos.
*          int size: Numero de bytes a recibir.
* DESCRIPCIÓN: Recibe datos a traves de un socket/conexion. La recepcion es bloqueante hasta obtener los datos.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int socket_receive(int socket_fd, char *data, int size){
	
	int i, num_chunks, nbytes, ret, nread = 0;
	
	if(data == NULL || size <= 0){
	    return -1;
	}
	
	/*Calculamos el numero de trozos*/
	num_chunks = (size-1) / CHUNK_SIZE +1;
	
	/*Los vamos leyendo sucesivamente*/
	for(i = 0; i < num_chunks; i++){
		/*Calculamos cuantos bytes van en este chunk*/
		if(i == (num_chunks-1)){
		    nbytes = size - CHUNK_SIZE*i;	
		} else {
			nbytes = CHUNK_SIZE;
		}
		
		/*Escribimos*/
		ret = read(socket_fd, data + i*CHUNK_SIZE, nbytes); 
		if(ret == -1){
			return nread;
		}
		
		nread += ret;
	}
	
	return nread;
	
}

/****
* FUNCIÓN: int socket_receive_nonblock(int socket_fd, char *data, int size)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion por el que recibir los datos.
*          char* data - comienzo del espacio de memoria donde se copiaran los datos.
*          int size: Numero de bytes a recibir.
* DESCRIPCIÓN: Recibe datos a traves de un socket/conexion. La recepcion es bloqueante hasta obtener los datos. La recepcion es no bloqueante.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito y -1 en caso de error.
****/
int socket_receive_nonblock(int socket_fd, char *data, int size){
    int i, num_chunks, nbytes, ret, nread = 0, flags;
	
	if(data == NULL || size <= 0){
	    return -1;
	}

        /*Asignamos al socket que sea no bloqueante*/
        flags = fcntl(socket_fd, F_GETFL);
        fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
	
	/*Calculamos el numero de trozos*/
	num_chunks = (size-1) / CHUNK_SIZE +1;
	
	/*Los vamos leyendo sucesivamente*/
	for(i = 0; i < num_chunks; i++){
		/*Calculamos cuantos bytes van en este chunk*/
		if(i == (num_chunks-1)){
		    nbytes = size - CHUNK_SIZE*i;	
		} else {
			nbytes = CHUNK_SIZE;
		}
		
		/*Escribimos*/
		ret = read(socket_fd, data + i*CHUNK_SIZE, nbytes); 
                /*Ante una lectura de 0 bytes salimos*/
		if(ret <= 0){
                        fcntl(socket_fd, F_SETFL, flags);
			return nread;
		}
		
		nread += ret;
	}
	
        fcntl(socket_fd, F_SETFL, flags & ~O_NONBLOCK);
	return nread;
}

/****
* FUNCIÓN: int socket_set_read_timer(int socket_fd, int seconds)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion afectado.
*          int seconds : valor en segundos del temporizador.
* DESCRIPCIÓN: Ajusta un temporizador al socket deseado, de tal manera
* que si se queda bloqueado en socket_receive mas del tiempo indicado,
* socket_receive devolvera -1 (o el numero de bytes transferidos hasta
* el momento) y ajustara el errno a EAGAIN o EWOULDBLOCK
* ARGS_OUT: int - Devuelve -1 en caso de error, 0 en caso contrario.
****/
int socket_set_read_timer(int socket_fd, int seconds){
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    return setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv, sizeof(struct timeval));
}
