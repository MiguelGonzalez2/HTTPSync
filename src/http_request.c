#include "http_request.h"
#include "libsocket.h"
#include <string.h>

struct _request_t{
    char *method;
    char *path;
    error errorType;
};

/****
*FUNCION: request_t* http_getRequest()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/
request_t* http_getRequest(int socket_fd){
    int btyes;
    char *requestString, *method, *path;
    size_t method_len, path_len;

    request_t *request = NULL;

    /*Reservamos memoria para la struct request*/   
    request = malloc(sizeof(request_t));
    if(request == NULL){
        syslog(LOG_ERR,"Error al reservar memoria para la struct request_t");
        return NULL;
    }

    /*Reservamos memoria para el buffer que contiene la request*/   
    requestString = malloc(sizeof(char) * MAX_REQUEST_LENGTH);
    if(requestString == NULL){
        free(request);
        syslog(LOG_ERR,"Error al reservar memoria para una cadena de caracteres");
        return NULL;
    }

    /*Obtenemos la request*/   
    bytes = socket_receive(socket_fd, requestString, MAX_REQUEST_LENGHT);
    if(bytes <= 0){
        free(request);
        free(requestString);
        syslog(LOG_ERR,"Error al leer del socket");
        return NULL;
    }

    /*Si se llena el buffer devolevemos el error RequestTooLong*/   
    if(bytes == MAX_REQUEST_LENGTH){
        request->errorType = error.RequestTooLong;
        free(requestString);
        return request;
    }

    /*Llamamos al picohttpparser*/   
    pret = phr_parse_request(requestString, bytes, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, prevbuflen);

    if(pret < 0){
        request->errorType = error.BadRequest;
    }

    /*Aniadimos el path y el metodo a la struct request*/
    request->method = malloc(sizeof(char) * ((int)metodh_led +1));
    request->path = malloc((sizeof(char) * ((int)path_len +1));

    strncpy(request->method, method, (int)method_len);
    request->method[method_len] = '\0'

    strncpy(request->path, path, (int)path_len);
    request->path[path_len] = '\0'

    free(requestString);

    return request;
}
