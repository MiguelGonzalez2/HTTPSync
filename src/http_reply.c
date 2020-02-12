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
#include <time.h>

/****
* FUNCIÓN: int http_reply_send(int conn_fd, request_t req)
* ARGS_IN: int conn_fd: Descriptor del socket al que enviar la reply.
*          request_t req: Peticion a la que responder.
* DESCRIPCIÓN: Recibe una peticion y responde segun lo solicitado.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int http_reply_send(int conn_fd, request_t req){
    
    /*Comprobar status de la request*/

    /*Abre el fichero, si no esta NOT FOUND, si no OK*/

    /*Leer fecha de modificacion y tamano*/

    /*Construir y mandar la cabecera*/

    /*Mandar el fichero*/

    /*Liberar*/
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
    for(i = strlen(path)-1; (i >= 0) && (extension == -1), i--){
        if(path[i] = '.'){
            extension = i;
        }
    }

    if(extension == 0 && path[0] != '.'){
        return NULL;
    }

    /*Decidimos el tipo*/
    if(!strcmp(path[i+1],"txt")){
        strcpy(content_type, "text/plain");
    } else if(!strcmp(path[i+1],"html") || !strcmp(path[i+1],"htm")){
        strcpy(content_type, "text/html");
    } else if (!strcmp(path[i+1],"gif")){
        strcpy(content_type, "image/gif");
    } else if(!strcmp(path[i+1],"jpeg") || !strcmp(path[i+1],"jpg")){
        strcpy(content_type, "image/jpeg");
    } else if(!strcmp(path[i+1],"mpeg") || !strcmp(path[i+1],"mpg")){
        strcpy(content_type, "video/mpeg");
    } else if(!strcmp(path[i+1],"doc") || !strcmp(path[i+1],"docx")){
        strcpy(content_type, "application/msword");
    } else if (!strcmp(path[i+1],"pdf")){
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
    char buf[256];
    time_t now = time(0);
    struct tm *tm = gmtime(&now);

    if(tm == NULL){
        return NULL;
    }

    /*Damos el formato adecuado*/
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", tm);
    return buf;
}
