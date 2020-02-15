#include "http_request.h"
#include "libsocket.h"
#include "libPicohttpparser.h"
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

struct _request_t{
    char *method;
    char *path;
    http_req_error errorType;
};

/****
*FUNCION: request_t* http_get_request()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/ request_t* http_get_request(int socket_fd){
    int bytes,err,minor_version,strCompareRes;
    char *requestString;
    const char *method, *path;
    size_t method_len, path_len,num_headers,prevbuflen = 0;
    struct phr_header headers[100];

    request_t *request = NULL;

    /*Reservamos memoria para la struct request*/   
    request = malloc(sizeof(request_t));
    if(request == NULL){
        syslog(LOG_ERR,"Error al reservar memoria para la struct request_t");
        return NULL;
    }

    request->method = NULL;
    request->path = NULL;
    request->errorType = BadRequest;

    /*Reservamos memoria para el buffer que contiene la request*/   
    requestString = malloc(sizeof(char) * MAX_REQUEST_LENGTH);
    if(requestString == NULL){
        free(request);
        syslog(LOG_ERR,"Error al reservar memoria para una cadena de caracteres");
        return NULL;
    }

    /*Obtenemos la request*/   
    bytes = socket_receive(socket_fd, requestString, MAX_REQUEST_LENGTH);
    if(bytes <= 0){
        free(request);
        free(requestString);
        syslog(LOG_ERR,"Error al leer del socket");
        return NULL;
    }

    /*Si se llena el buffer devolevemos el error RequestTooLong*/   
    if(bytes == MAX_REQUEST_LENGTH){
        request->errorType = RequestTooLong;
        free(requestString);
        return request;
    }

    /*Llamamos al picohttpparser*/   
    err = phr_parse_request(requestString, bytes, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, prevbuflen);

    if(err < 0){
        free(requestString);
        return request;
    }

    /*Aniadimos el path y el metodo a la struct request*/
    request->method = malloc(sizeof(char) * ((int)method_len + 1));
    request->path = malloc(sizeof(char) * ((int)path_len + 1));

    strncpy(request->method, method, (int)method_len);
    request->method[method_len] = '\0';

    strncpy(request->path, path, (int)path_len);
    request->path[path_len] = '\0';
 
    /*Reemplazamos el path '/' */
    strCompareRes = strcmp(request->path, "/");
    if(!strCompareRes){
        if(realloc(request->path,strlen(DEFAULT_URI) + 1) == NULL){
            http_req_destroy(request);
            return NULL;
        }
        strcpy(request->path,DEFAULT_URI);
    }

    request->errorType = OK;

    free(requestString);

    return request;
}

/****
*FUNCION: char *http_get_method(request *req)
*ARGS_IN: request *req: Estructura request de la que se pide el metodo
*DESCRIPCION: Devuelve el metodo de una request
*ARGS_OUT: char* req->method: String con el metodo
****/ 
char *http_get_method(request_t *req){
	
	if(req == NULL){
		return NULL;
	}

    return req->method;
}

/****
*FUNCION: char *http_get_path(request_t *req)
*ARGS_IN: request_t *req: Estructura request de la que se pide el path
*a un fichero.
*DESCRIPCION: Devuelve el path al fichero solicitado por la request.
*ARGS_OUT: cha* req->method: String con el path.
****/ 
char *http_get_path(request_t *req){

	if(req == NULL){
		return NULL;
	}

    return req->path;
}

/****
*FUNCION: http_req_error http_get_error(request_t *req);
*ARGS_IN: request_t *req: Estructura request de la que se pide 
*el errorType
*DESCRIPCION: Devuelve el tipo de error de una request,
*OK en caso de no haber error.
*ARGS_OUT: http_req_error req->errorType: El error a devolver.
****/ 
http_req_error http_get_error(request_t *req){

	if(req == NULL){
		return BadRequest;
	}

    return req->errorType;
}

/****
*FUNCION: void http_req_destroy(request_t *req)
*ARGS_IN: request_t *req: Estructura request a liberar.
*DESCRIPCION:
*ARGS_OUT: none
****/ 
void http_req_destroy(request_t *req){

    if(req == NULL){
        return;
    }

    if(req->path != NULL){
        free(req->path);
    }

    if(req->method != NULL){
        free(req->method);
    }

    free(req);

    return;
}

