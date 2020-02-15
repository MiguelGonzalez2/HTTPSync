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

/****
*FUNCION: request_t* http_getRequest()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/
request_t* http_get_request(int socket_fd);

/*TODO SI EL PATH ES '/', EN LUGAR DE ESO; REEMPLAZARLO POR LA MACRO DEFAULT_URI*/
/*TODO IMPLEMENTAR ESTAS FUNCIONES Y COMENTARLAS.*/
char *http_get_method(request_t *req);
char *http_get_path(request_t *req);
http_req_error http_get_error(request_t *req);
int http_req_destroy(request_t *req);


#endif

