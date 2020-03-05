#include "http_request.h"
#include "libsocket.h"
#include "libPicohttpparser.h"
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>

struct _request_t{
    char *method;
    char *path;
    char *args;
    http_req_error errorType;
    connectionStatus connection;
};

/****
*FUNCION: request_t* http_get_request()
*ARGS_IN: int socket_fd: Descriptor del socket/conexion del que parsear la request
*         char *web_dir: directorio raiz del servidor
*DESCRIPCION: Crea una estructura request.
*ARGS_OUT: Devuelve un puntero a una estructura request_t. Si hay un error se indica en
*la variable errorType de la estructura.
****/
request_t* http_get_request(int socket_fd, char *web_dir){
    int i, bytes,err,minor_version,strCompareRes,connFound = 0, web_dir_len, real_path_len, args_len;
    char *requestString=NULL;
    const char *method=NULL, *path=NULL, *args=NULL;
    size_t method_len=0, path_len=0,num_headers=0,prevbuflen = 0;
    struct phr_header headers[100];

    request_t *request = NULL;

    if(web_dir == NULL){
        syslog(LOG_ERR,"HTTPServer: Error: directorio de la web no especificado.");
        return NULL;
    }

    web_dir_len = strlen(web_dir);

    /*Reservamos memoria para la struct request*/   
    request = malloc(sizeof(request_t));
    if(request == NULL){
        syslog(LOG_ERR,"HTTPServer: Error al reservar memoria para la struct request_t.");
        return NULL;
    }

    request->method = NULL;
    request->path = NULL;
    request->args = NULL;
    request->errorType = BadRequest;
    request->connection = Close;

    /*Reservamos memoria para el buffer que contiene la request.*/
    /*Reservamos 4 mas de lo necesario porque posteriormente moveremos
      un puntero que podria salirse si se llena este buffer*/   
    requestString = malloc(sizeof(char) * (MAX_REQUEST_LENGTH+4));
    if(requestString == NULL){
        free(request);
        syslog(LOG_ERR,"Error al reservar memoria para una cadena de caracteres.");
        return NULL;
    }

    /*Inicializamos la cadena*/
    memset(requestString, 0, MAX_REQUEST_LENGTH+4);

    /*Obtenemos la request*/   
    bytes = socket_receive(socket_fd, requestString, MAX_REQUEST_LENGTH);
    if(bytes <= 0){
        free(request);
        free(requestString);
        /*Solo se trata de un error cuando no sea un timeout ni un SIGINT*/
        if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK){
        	syslog(LOG_ERR,"HTTPServer: Error al leer del socket. %d bytes",bytes);
        }
        return NULL;
    }

    /*Si se llena el buffer devolevemos el error RequestTooLong*/   
    if(bytes == MAX_REQUEST_LENGTH){
        request->errorType = RequestTooLong;
        syslog(LOG_ERR,"HTTPServer: Tamanio de la request demasiado grande.");
        
        /*Limpiamos el buffer*/
        do{
            bytes = socket_receive_nonblock(socket_fd, requestString, MAX_REQUEST_LENGTH);
            if(bytes < 0){
                free(request);
                free(requestString);
                /*Solo se trata de un error cuando no sea un timeout ni un SIGINT*/
                if(errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK){
                    syslog(LOG_ERR,"HTTPServer: Error limpiando el socket");
                }
                return NULL;
            }
            
        }while(bytes != 0);

        free(requestString);
        return request;
    }

    /*Llamamos al picohttpparser*/
    num_headers = sizeof(headers) / sizeof(headers[0]);   
    err = phr_parse_request(requestString, bytes, &method, &method_len, &path, &path_len,
                             &minor_version, headers, &num_headers, prevbuflen);

    if(err ==-1){
        free(requestString);
        syslog(LOG_ERR, "HTTPServer: Error de PicoHTTPParser with code %d", err);
        return request;
    }

    /*Eliminamos la barra final del web_dir si la hay*/
    if(web_dir[web_dir_len - 1] == '/'){
        web_dir[web_dir_len - 1] = 0;
        web_dir_len--;
    }

    real_path_len = (int)path_len;

    /*Encontramos donde esta el ? del path. Buscamos como mucho hasta que se acabe el path (espacio)*/
    for(args = path; *args != ' ' && *args != '?' && *args != 0;args++);

    /*Aniadimos los args*/
    if(strncmp(method,"GET",3) == 0){
        /*Habia parametros*/
        if(*args == '?'){
            real_path_len = args - path;
            args_len = (int)path_len - real_path_len - 1;

            request->args = malloc(sizeof(char) * (args_len + 1));

            strncpy(request->args , args + 1, args_len);
            request->args[args_len] = '\0';
        }

    }else if(strncmp(method,"POST",4) == 0){
        /*Eliminamos del path lo que haya tras un '?', aunque no lo almacenemos*/
        if(*args == '?') real_path_len = args - path;

        /*value apunta al comienzo del valor del ultimo header. Nos movemos hasta el final sumando*/
        /*lo que ocupa dicho header, y despues sumamos 4 dado que hay 2 \r\n a continuacion.*/
        args =  headers[num_headers - 1].value + headers[num_headers - 1].value_len + 4;

        /*Procesamos solo si habia argumentos*/
        if(*args != 0){
            request->args = malloc(sizeof(char) * (strlen(args) + 1));
            strcpy(request->args , args);
        }
    }

    /*Reservamos memoria para el metodo y el path de la struct request*/
    request->method = malloc(sizeof(char) * ((int)method_len + 1));
    request->path = malloc(sizeof(char) * ((int)real_path_len + 1 + web_dir_len));

    /*Metemos en la struct request el metodo, el path y los argumentos*/
    strncpy(request->method, method, (int)method_len);
    request->method[method_len] = '\0';

    strncpy(request->path, web_dir, web_dir_len);
    strncpy(request->path + web_dir_len, path, real_path_len);
    request->path[real_path_len + web_dir_len] = '\0';

 
    /*Reemplazamos el path '/' */
    strCompareRes = strcmp(request->path + web_dir_len, "/");
    if(!strCompareRes){
        request->path = realloc(request->path,strlen(DEFAULT_URI) + 1 + web_dir_len);
        if(request->path == NULL){
            http_req_destroy(request);
            return NULL;
        }

        strcpy(request->path + web_dir_len,DEFAULT_URI);
    }

    /*Valores asignados cuando no hay errores*/
    request->errorType = OK;
    request->connection = Open;

    /*Buscamos Connection: Close en los headers*/
    for(i = 0; i < num_headers && !connFound; i++){
        strCompareRes = strncmp(headers[i].name, "Connection", headers[i].name_len);
        if(!strCompareRes){
            strCompareRes = strncmp(headers[i].value, "close", headers[i].value_len);
            if(!strCompareRes){
                request->connection = Close;
            }

            /*Si hemos encontrado el header con nombre Connection salimos*/
            connFound = 1;
        }
    }

    /*Por seguridad: Impedimos /../ en la URI puesto que
      permitiria acceder a contenidos en directorios ajenos al server*/
    if(strstr(request->path, "/../") != NULL){
        request->errorType = BadRequest;
    }

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
*FUNCION: char *http_get_args(request_t *req)
*ARGS_IN: request_t *req: Estructura request de la que se pide los argumentos.
*DESCRIPCION: Devuelve los argumentos introducidos en la request.
*ARGS_OUT: char* args:  String con los args.
****/ 
char *http_get_args(request_t *req){

	if(req == NULL){
		return NULL;
	}

    return req->args;
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
*FUNCION: http_req_connection_status http_get_error(request_t *req)
*ARGS_IN: request_t *req: Estructura request de la que se pide
*el errorType
*DESCRIPCION: Devuelve el tipo de error de una request,
*OK en caso de no haber error.
*ARGS_OUT: connectionStatus: Estado de la conexion.
****/
connectionStatus http_get_connection_status(request_t *req){
   
    if(req == NULL){
	return Close;
    }

    return req->connection;
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

    if(req->args != NULL){
        free(req->args);
    }

    free(req);

    return;
}

