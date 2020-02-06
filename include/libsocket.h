/**
* Libreria de gestion de sockets TCP. Proporciona una interfaz
* comoda para abrir un socket TCP en modo escucha y gestionar
* conexiones.
*
* @author Miguel Gonzalez, Alejandro Bravo
* @version 1.0
* @date 06-02-2020
*/

#ifndef LIBSOCKET_H
#define LIBSOCKET_H

#define CHUNK_SIZE 4096 /*!< Cantidad de bytes que se escriben/leen de golpe al socket como maximo*/

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
int socket_server_init(char *ip, int port, int listen_size);

/****
* FUNCIÓN: int socket_close(int socketfd)
* ARGS_IN: int fd - Descriptor del socket o conexion a cerrar.
* DESCRIPCIÓN: Cierra un socket o una conexion, liberando los recursos asociados.
* ARGS_OUT: int - Devuelve 0 si se ha cerrado correctamente, o -1 en caso contrario.
****/
int socket_close(int fd);

/****
* FUNCIÓN: int socket_accept(int socket_fd)
* ARGS_IN: int socket_fd: Descriptor del servidor del que se acepta la conexion.
*          int *port: Direccion de memoria donde se escribira el puerto del cliente. En orden usual (no de red).
*          int *addr: Direccion de memoria donde se escribira el valor de la IP origen. En orden usual (no de red).
* DESCRIPCIÓN: Establece la conexion con un cliente. Es bloqueante hasta que se reciba una peticion de conexion.
* ARGS_OUT: int - Devuelve el identificador (descriptor) de la conexion con el cliente, o -1 en caso de error.
****/
int socket_accept(int socket_fd, int *port, int *addr);

/****
* FUNCIÓN: int socket_send(char *data, int size)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion a la que enviar los datos.
*          char* data - comienzo de los bytes que se desean transferir.
*          int size: Numero de bytes a transferir.
* DESCRIPCIÓN: Envia datos a traves de un socket/conexion.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos, o -1 en caso de otro error.
****/
int socket_send(int socket_fd, char *data, int size);

/****
* FUNCIÓN: int socket_receive(int socket_fd, char *data, int size)
* ARGS_IN: int socket_fd: Descriptor del socket/conexion por el que recibir los datos.
*          char* data - comienzo del espacio de memoria donde se copiaran los datos.
*          int size: Numero de bytes a recibir.
* DESCRIPCIÓN: Recibe datos a traves de un socket/conexion. La recepcion es bloqueante hasta obtener los datos.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int socket_receive(int socket_fd, char *data, int size);
#endif