/**
*
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 12-02-2020

**/
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#define MAX_REQUEST_LENGTH 2048
typedef struct _request_t request_t;

enum error{BadRequest, RequestTooLong}; 

/****
*FUNCION: request_t* http_getRequest()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/
request_t* http_getRequest(int socket_fd);

#endif

