/**
* Fuente del modulo que gestiona las respuestas http.
*
* @author Miguel Gonzalez, Alejandro Bravo
* @version 1.0
* @date 06-02-2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libsocket.h"
#include "http_reply.h"

char *http_reply_gettype(char *path);
int http_send_file(int socket_fd, char *path);
int http_file_size(char *path);
int http_file_exists(char *path);
char *http_reply_get_last_modified(char *path);
char *http_reply_getdate();

/****
* FUNCIÓN: int http_reply_send(int conn_fd, request_t *req)
* ARGS_IN: int conn_fd: Descriptor del socket al que enviar la reply.
*          request_t req: Peticion a la que responder. Si es NULL la respuesta
*          contendra informacion minima (fecha y nombre), y un error (400 o 408 si es timeout)
*          connectionStatus connection_close: Si esta puesto a "Close", notifica
*          al receptor de cierre de conexion. Si esta puesto a "TimedOut", notifica de
*          temporizador.
* DESCRIPCIÓN: Recibe una peticion y responde segun lo solicitado.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int http_reply_send(int conn_fd, request_t *req, connectionStatus connection_close){
    
    /*Cabecera del mensaje HTTP*/
    char header[2048]; 
    char *path=NULL;
    char *method = NULL;
    char *buffer = NULL;
    http_req_error err = BadRequest;
    /*Determina si se trata de OPTIONS * que no trata sobre un fichero*/
    int options = 0, size = 0;

    memset(header, 0, 2048);
   
    /*Anadimos la version HTTP*/
    strcat(header, "HTTP/1.1 ");

    /*Comprobar status de la request*/
    if(req != NULL){
        err = http_get_error(req);
        method = http_get_method(req);
        path = http_get_path(req);
    }

    /*Preparamos el path relativo quitando la barra*/
    if(path != NULL && path[0] == '/'){
            path ++;
    }

    if(err == OK){
        options = (!strcmp(method, "OPTIONS") && !strcmp(path, "*")) ? 1 : 0;
    }

    /*Codigos de estados*/
    if(err == OK){
        if(options || http_file_exists(path)){
            strcat(header, "200 OK\r\n");
        } else {
            strcat(header, "404 Not Found\r\n");
            /*Quitamos OK del err*/
            err = BadRequest;
        }    
    } else if (connection_close == TimedOut){
        strcat(header, "408 Request Timeout\r\n");
    } else if(err == BadRequest){
        strcat(header, "400 Bad Request\r\n");
    } else {
        strcat(header, "500 Internal Server Error\r\n");
    }

    /*Anadimos Fecha*/
    buffer = http_reply_getdate();
    if(buffer != NULL){
	    strcat(header, "Date: ");
	    strcat(header, buffer);
	    free(buffer);
    }

    /*Aniadimos datos del server*/
    strcat(header, "Server: HTTPServer by A.B and M.G\r\n");

    if(!options && err==OK){
        /*Fecha de modificacion*/
        buffer = http_reply_get_last_modified(path);
        if(buffer != NULL){
		strcat(header, "Last-Modified: ");
		strcat(header, buffer);
		free(buffer);
        }
        /*Tipo de contenido*/
        buffer = http_reply_gettype(path);
        if(buffer != NULL){
                strcat(header, "Content-Type: ");
                strcat(header, buffer);
                free(buffer);
        }
        /*Tamano del contenido*/
        if(strcmp(method,"OPTIONS")){
		size = http_file_size(path);
		strcat(header, "Content-Length: ");
		buffer = malloc(64*sizeof(char));
		if(buffer != NULL){
		    sprintf(buffer, "%d\r\n", size);
		    strcat(header, buffer);
		    free(buffer);
		}
        } else {
             /*Es OPTIONS, No se envia contenido*/
             strcat(header, "Content-Length: 0\r\n");
        }
       
    } else {
         strcat(header, "Content-Length: 0\r\n");
    } 

    if(connection_close!=Open){
            strcat(header, "Connection: close\r\n");
    }


    if(err == OK &&!strcmp(method, "OPTIONS")){
        strcat(header, "Allow: GET,POST,OPTIONS\r\n");
    }
   
    /*Fin de la cabecera*/
    strcat(header, "\r\n");

    /*Mandamos la cabecera*/
    size = socket_send(conn_fd, header, strlen(header));

    /*Mandar el fichero*/
    if(err==OK && !options && strcmp(method, "OPTIONS")){
        size += http_send_file(conn_fd, path);
    }

    return size;
}

/****
* FUNCIÓN: char *http_reply_gettype(char *path)
* ARGS_IN: char *path: path del fichero que se ha solicitado, acabado en \0.
* DESCRIPCIÓN: Devuelve un char con el campo Content-Type que se debe copiar.
* Viene ya con el \r\n.
* ARGS_OUT: char *Direccion de memoria que apunta a la cadena correspondiente.
* devuelve NULL en caso de error. La memoria reservada debera liberarse.
****/
char *http_reply_gettype(char *path){

    char *content_type;
    int i, extension = -1;

    if(path == NULL){
        return NULL;
    }

    /*Basta con 32B para todos los textos posibles*/
    content_type = (char *) malloc(sizeof(char)*32);
    if(content_type == NULL){
        return NULL;
    }

    /*Detecta donde comienza la extension*/
    for(i = strlen(path)-1; (i >= 0) && (extension == -1); i--){
        if(path[i] == '.'){
            extension = i;
        }
    }

    /*Caso en el que no hay extensiones*/
    if(extension == 0 && path[0] != '.'){
        return NULL;
    }
 
    /*Apuntamos al inicio de la extension*/
    i = extension;

    /*Decidimos el tipo*/
    if(!strcmp(&path[+1],"txt")){
        strcpy(content_type, "text/plain");
    } else if(!strcmp(&path[i+1],"html") || !strcmp(&path[i+1],"htm")){
        strcpy(content_type, "text/html");
    } else if (!strcmp(&path[i+1],"gif")){
        strcpy(content_type, "image/gif");
    } else if(!strcmp(&path[i+1],"jpeg") || !strcmp(&path[i+1],"jpg")){
        strcpy(content_type, "image/jpeg");
    } else if(!strcmp(&path[i+1],"mpeg") || !strcmp(&path[i+1],"mpg")){
        strcpy(content_type, "video/mpeg");
    } else if(!strcmp(&path[i+1],"doc") || !strcmp(&path[i+1],"docx")){
        strcpy(content_type, "application/msword");
    } else if (!strcmp(&path[i+1],"pdf")){
        strcpy(content_type, "application/pdf");
    } else {
        return NULL;
    }

    i = strlen(content_type);
    content_type[i] = '\r';
    content_type[i+1] = '\n';
    content_type[i+2] = '\0';

    return content_type;
    
}

/****
* FUNCIÓN: char *http_reply_getdate()
* DESCRIPCIÓN: Devuelve un char con el campo date que se debe copiar.
* Viene ya con el \r\n.
* ARGS_OUT: char *Direccion de memoria que apunta a la cadena correspondiente.
* devuelve NULL en caso de error. La memoria reservada debera liberarse.
****/
char *http_reply_getdate(){
    char *buf;
    time_t now;
    struct tm *tm;

    now = time(0);
    tm = gmtime(&now);
    buf = malloc(256*sizeof(char));

    if(tm == NULL || buf == NULL){
        return NULL;
    }

    /*Damos el formato adecuado*/
    strftime(buf, 256*sizeof(char), "%a, %d %b %Y %H:%M:%S %Z\r\n", tm);
    return buf;
}

/****
* FUNCIÓN: char *http_reply_get_last_modified(char *path)
* DESCRIPCIÓN: Devuelve un char con el campo date que se debe copiar.
* Viene ya con el \r\n.
* ARGS_IN: char *fichero del que obtener la fecha.
* ARGS_OUT: char *Direccion de memoria que apunta a la cadena correspondiente.
* devuelve NULL en caso de error. La memoria reservada debera liberarse.
****/
char *http_reply_get_last_modified(char *path){
    char *buf;
    struct tm *tm;
    struct stat attrib;

    if(path == NULL){
        return NULL;
    }

    stat(path, &attrib);
  
    buf = malloc(256*sizeof(char));
    tm = gmtime(&(attrib.st_mtime));

    if(tm == NULL || buf == NULL){
        return NULL;
    }

    /*Damos el formato adecuado*/
    strftime(buf, 256*sizeof(char), "%a, %d %b %Y %H:%M:%S %Z\r\n", tm);
    return buf;
}

/****
* FUNCIÓN: http_file_exists( char * path)
* DESCRIPCIÓN: Indica si existe un recurso.
* ARGS_IN: char *path: Ruta del recurso.
* ARGS_OUT: 0 si el recurso no existe, 1 si existe.
****/
int http_file_exists(char *path){

    if(path == NULL){
        return 0;
    }

    return (access(path,F_OK) == 0);
}

/****
* FUNCIÓN: http_file_size(char *path)
* DESCRIPCIÓN: Da el tamano en bytes de un fichero.
* ARGS_IN: char *path: Ruta del recurso.
* ARGS_OUT: Tamano en bytes del fichero, 0 si falla.
****/
int http_file_size(char *path){

    struct stat attrib;

    if(path == NULL){
        return 0;
    }

    stat(path, &attrib);
    return attrib.st_size;
}

/****
* FUNCIÓN: http_send_file(int socket_fd, char *path)
* DESCRIPCIÓN: Envia un fichero por el socket.
* ARGS_IN: char *path: Ruta del recurso.
*          int socket_fd : Descriptor del socket.
* ARGS_OUT: Numero de bytes enviados.
****/
int http_send_file(int socket_fd, char *path){
        
    FILE *f;
    int read_size, total;
    char buffer[8192];

    total = 0;
    f = fopen(path, "r");
    if(f != NULL){	
        while((read_size = fread(buffer, sizeof(char), 8192, f)) > 0){
            total += socket_send(socket_fd, buffer, read_size);
        }
    }
    fclose(f);
    return total;
}
