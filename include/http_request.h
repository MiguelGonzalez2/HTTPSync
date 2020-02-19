/**
*
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 12-02-2020

**/
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#define MAX_REQUEST_LENGTH 2048 /*!<Maxima longitud para una request*/
#define DEFAULT_URI "/index.html" /*!<Objeto por defecto en la raiz*/
typedef struct _request_t request_t;

typedef enum {OK, BadRequest, RequestTooLong} http_req_error; 
typedef enum {Open, Close} connectionStatus;

/****
*FUNCION: request_t* http_getRequest()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/
request_t* http_get_request(int socket_fd);

/****
*FUNCION: char *http_get_method(request *req)
*ARGS_IN: request *req: Estructura request de la que se pide el metodo
*DESCRIPCION: Devuelve el metodo de una request
*ARGS_OUT: char* req->method: String con el metodo
****/
char *http_get_method(request_t *req);

/****
*FUNCION: char *http_get_path(request_t *req)
*ARGS_IN: request_t *req: Estructura request de la que se pide el path
*a un fichero.
*DESCRIPCION: Devuelve el path al fichero solicitado por la request.
*ARGS_OUT: cha* req->method: String con el path.
****/
char *http_get_path(request_t *req);

/****
*FUNCION: http_req_error http_get_error(request_t *req);
*ARGS_IN: request_t *req: Estructura request de la que se pide
*el errorType
*DESCRIPCION: Devuelve el tipo de error de una request,
*OK en caso de no haber error.
*ARGS_OUT: http_req_error req->errorType: El error a devolver.
****/
http_req_error http_get_error(request_t *req);

/****
*FUNCION: http_req_connection_status http_get_error(request_t *req)
*ARGS_IN: request_t *req: Estructura request de la que se pide
*el errorType
*DESCRIPCION: Devuelve el tipo de error de una request,
*OK en caso de no haber error.
*ARGS_OUT: connectionStatus: Estado de la conexion.
****/
connectionStatus http_get_connection_status(request_t *req);

/****
*FUNCION: void http_req_destroy(request_t *req)
*ARGS_IN: request_t *req: Estructura request a liberar.
*DESCRIPCION:
*ARGS_OUT: none
****/
void http_req_destroy(request_t *req);

#endif

