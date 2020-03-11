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
#include <sys/prctl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include "libsocket.h"
#include "http_reply.h"
#include "errno.h"

typedef enum {NONE, PHP, PYTHON3} script_type;

char *http_reply_gettype(char *path, script_type *script);
int http_send_file(int socket_fd, char *path);
int http_file_size(char *path);
int http_file_exists(char *path);
char *http_reply_get_last_modified(char *path);
char *http_reply_getdate();
char *exec_script(char *path, script_type script, char *input);
time_t http_date_to_int(char *date);

/****
* FUNCIÓN: int http_reply_send(int conn_fd, request_t *req)
* ARGS_IN: int conn_fd: Descriptor del socket al que enviar la reply.
*          request_t req: Peticion a la que responder. Si es NULL la respuesta
*          contendra informacion minima (fecha y nombre), y un error (400 o 408 si es timeout)
*          connectionStatus connection_close: Si esta puesto a "Close", notifica
*          al receptor de cierre de conexion. Si esta puesto a "TimedOut", notifica de
*          temporizador.
*          char *server_name : nombre del servidor
* DESCRIPCIÓN: Recibe una peticion y responde segun lo solicitado.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int http_reply_send(int conn_fd, request_t *req, connectionStatus connection_close, char *server_name){
    
    /*Cabecera del mensaje HTTP*/
    char header[2048]; 
    char *path=NULL, *method = NULL, *args = NULL, *ifModifiedSince = NULL;
    char *buffer = NULL, *content_length = NULL;
    http_req_error err = BadRequest;
    script_type script = NONE;
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
        args = http_get_args(req);
        ifModifiedSince = http_get_ifModifiedSince(req);
    }

    if(err == OK){
        options = (!strcmp(method, "OPTIONS") && !strcmp(path, "*")) ? 1 : 0;
    }

    /*Comprobamos si no hace falta reenviar el recurso*/
    if(path != NULL){
        buffer = http_reply_get_last_modified(path);
    }

    if(ifModifiedSince != NULL && buffer != NULL && http_date_to_int(ifModifiedSince) > http_date_to_int(buffer)){
        /*Quitamos OK del err*/
        err = NotModified;
    }

    /*Codigos de estados*/
    if(err == OK){
        if(strcmp(method, "GET") && strcmp(method, "POST") && strcmp(method, "OPTIONS")){
            strcat(header, "501 Not Implemented\r\n");
            err = BadRequest;
        } else if(options || http_file_exists(path)){
            strcat(header, "200 OK\r\n");
        }else {
            strcat(header, "404 Not Found\r\n");
            /*Quitamos OK del err*/
            err = BadRequest;
        }    
    } else if (connection_close == TimedOut){
        strcat(header, "408 Request Timeout\r\n");
    } else if(err == BadRequest){
        strcat(header, "400 Bad Request\r\n");
    } else if(err == NotModified){
            strcat(header, "304 Not Modified\r\n");

    }else {
        strcat(header, "500 Internal Server Error\r\n");
    }

    if(buffer != NULL){
        free(buffer);
    }

    buffer = http_reply_getdate();
    if(buffer != NULL){
	    strcat(header, "Date: ");
	    strcat(header, buffer);
	    free(buffer);
    }

    /*Aniadimos datos del server*/
    strcat(header, "Server: ");
    strcat(header, server_name);
    strcat(header, "\r\n");

    if(!options && err==OK){
        /*Fecha de modificacion*/
        buffer = http_reply_get_last_modified(path);
        if(buffer != NULL){
		    strcat(header, "Last-Modified: ");
		    strcat(header, buffer);
		    free(buffer);
        }
        /*Tipo de contenido*/
        buffer = http_reply_gettype(path, &script);
        if(buffer != NULL){
                strcat(header, "Content-Type: ");
                strcat(header, buffer);
                free(buffer);
        }
        /*Tamano del contenido*/
        if(strcmp(method,"OPTIONS") && script == NONE){
		size = http_file_size(path);
		strcat(header, "Content-Length: ");
		content_length = malloc(64*sizeof(char));
		if(content_length != NULL){
		    sprintf(content_length, "%d\r\n", size);
		    strcat(header, content_length);
		    free(content_length);
		}
        /*Si es un script calculamos el tamano y dejamos el resultado en buffer*/
        } else if (script != NONE){
               /*Ejecucion del script*/
               buffer = exec_script(path, script,args);
               if(buffer == NULL){
                   strcat(header, "Content-Length: 0\r\n");
               } else {
                   strcat(header, "Content-Length: ");
                   content_length = malloc(64*sizeof(char));
		   if(content_length != NULL){
		       sprintf(content_length, "%d\r\n", (int) strlen(buffer));
		       strcat(header, content_length);
		       free(content_length);
		   }
               }
        } else {
             /*Es OPTIONS, No se envia contenido*/
             strcat(header, "Content-Length: 0\r\n");
        }
       
    } else {
         strcat(header, "Content-Length: 0\r\n");
    } 

    /*Informamos de que se vamos a efectuar un cierre de conexion*/
    if(connection_close!=Open){
            strcat(header, "Connection: close\r\n");
    }

    /*Anadimos los verbos disponibles en caso de que sea OPTIONS*/
    if(err == OK &&!strcmp(method, "OPTIONS")){
        strcat(header, "Allow: GET,POST,OPTIONS\r\n");
    }
   
    /*Fin de la cabecera*/
    strcat(header, "\r\n");

    /*Mandamos la cabecera*/
    size = socket_send(conn_fd, header, strlen(header));

    /*Mandar el fichero*/
    if(err==OK && !options && strcmp(method, "OPTIONS") && script == NONE){
        size += http_send_file(conn_fd, path);
    } else if (script != NONE && buffer != NULL){
        size += socket_send(conn_fd, buffer, strlen(buffer));
        free(buffer);
    }

    return size;
}

/****
* FUNCIÓN: char *http_reply_gettype(char *path)
* ARGS_IN: char *path: path del fichero que se ha solicitado, acabado en \0.
*          script_type *is_script: Tipo de script si se trata de uno, NONE si no.
* DESCRIPCIÓN: Devuelve un char con el campo Content-Type que se debe copiar.
* Viene ya con el \r\n.
* ARGS_OUT: char *Direccion de memoria que apunta a la cadena correspondiente.
* devuelve NULL en caso de error. La memoria reservada debera liberarse.
****/
char *http_reply_gettype(char *path, script_type *script){

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
        free(content_type);
        return NULL;
    }

    /*Por defecto no es script*/
    *script = NONE;
 
    /*Apuntamos al inicio de la extension*/
    i = extension;

    /*Decidimos el tipo*/
    if(!strcmp(&path[i+1],"txt")){
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
    } else if (!strcmp(&path[i+1],"py")){
        strcpy(content_type, "text/plain");
        *script = PYTHON3;
    } else if (!strcmp(&path[i+1],"php")){
        strcpy(content_type, "text/plain");
        *script = PHP;
    } else {
        free(content_type);
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
* FUNCIÓN: time_t http_date_to_int(char *date)
* DESCRIPCIÓN: Convierte una cadena de fecha en formato HTTP a un entero time_t
* ARGS_IN; char* date: Comienzo de la cadena en formato HTTP. 
*                      La cadena debe constar exclusivamente del texto de la fecha,
*                      NO debe tener caracteres adicionales (\r\n, etc). DEBE haber
*                      un byte nulo que indique el final de la cadena.
*                      Ejemplo: "Tue, 15 Nov 1994 08:12:31 GMT"
* ARGS_OUT: time_t: Valor numerico time_t que representa la fecha.
****/
time_t http_date_to_int(char *date){
    time_t time1, time2, auxt;
    struct tm *tm,*aux;
    char month[10];

    /*Apuntamos a la estructura*/ 
    time1 = 0;
    tm = gmtime(&time1);

    /*Cargamos la info de la cadena*/
    sscanf(date,  "%*c%*c%*c, %2d %s %d %2d:%2d:%2d %*s", &tm->tm_mday,month,&tm->tm_year,&tm->tm_hour,&tm->tm_min,&tm->tm_sec);

    /*No hay mas remedio que parsear el mes*/
    if(!strcmp(month,"Jan")) tm->tm_mon = 0;
    else if(!strcmp(month,"Feb")) tm->tm_mon = 1;
    else if(!strcmp(month,"Mar")) tm->tm_mon = 2;
    else if(!strcmp(month,"Apr")) tm->tm_mon = 3;
    else if(!strcmp(month,"May")) tm->tm_mon = 4;
    else if(!strcmp(month,"Jun")) tm->tm_mon = 5;
    else if(!strcmp(month,"Jul")) tm->tm_mon = 6;
    else if(!strcmp(month,"Aug")) tm->tm_mon = 7;
    else if(!strcmp(month,"Sep")) tm->tm_mon = 8;
    else if(!strcmp(month,"Oct")) tm->tm_mon = 9;
    else if(!strcmp(month,"Nov")) tm->tm_mon = 10;
    else if(!strcmp(month,"Dec")) tm->tm_mon = 11;

    /*Mas campos*/
    tm->tm_year -= 1900;
    tm->tm_isdst = 0;
    tm->tm_yday = 0;
    tm->tm_wday = 0;
    time2 = mktime(tm);

    /*Ahora time2 esta en zona local, hay que reconvertir*/

    /*Calculo el offset a GMT usando un tiempo prefijado*/
    time1 = time(0);
    aux = gmtime(&time1);
    auxt = mktime(aux);
    aux = localtime(&time1);
    time1 = difftime(auxt,mktime(aux));

    /*Arreglamos ese offset*/
    time2 -= time1;

    return time2;
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

    if(path == NULL || !http_file_exists(path)){
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

/****
* FUNCIÓN: exec_script(char *path, script_type script)
* DESCRIPCIÓN: Ejecuta un script y devuelve la respuesta.
* ARGS_IN: char *path: Ruta del recurso.
*          script_type script: Tipo del script (interprete a usar).
*          char *input: Bytes that will be transferred to the script's standard input after launching.
* ARGS_OUT: Salida del script (reserva memoria), o bien NULL en caso de error.
****/
char *exec_script(char *path, script_type script, char *input){
    pid_t pid = 0;
    /*Pipe del que escribiremos*/
    int wpipe[2];
    /*Pipe del que leeremos*/
    int rpipe[2];
    int end = 0;
    int nread, total_read = 0;
    char interpreter[128] = "";
    char *buffer = NULL, *pointer = NULL;
    fd_set set;
    struct timeval tv;
    
    /*Elaboramos el comando*/
    switch (script){
        case PHP:
            strcat(interpreter, "php");
            break;
        case PYTHON3:
            strcat(interpreter, "python3");
            break;
        default:
            return NULL;
    }
    
    /*Abrimos las tuberias*/
    if(pipe(wpipe) == -1 || pipe(rpipe) == -1) return NULL;

    /*Creamos un proceso*/
    pid = fork();

    if(pid < 0){
        return NULL;
    }
    
    /*Codigo del hijo*/
    if(pid == 0){
        
        close(wpipe[1]);
        close(rpipe[0]);

        /*Enlazamos los descriptores estandar a los pipes*/
        dup2(wpipe[0], STDIN_FILENO);
        dup2(rpipe[1], STDOUT_FILENO);
        dup2(rpipe[1], STDERR_FILENO);

        /*Si el server se cierra, se mandara un SIGTERM al script para que no quede suelto*/
        /*En principio un cierre normal del server esperara a que este hilo acabe, pero*/
        /*si el server se cierra bruscamente con SIGKILL desde fuera podria pasar.*/
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        
        /*Se lanza el proceso*/
        execlp(interpreter, interpreter, path, (char *) NULL);
        /*Si falla el exec salimos*/
        exit(EXIT_FAILURE);
    }

    /*Codigo principal (padre)*/
    close(wpipe[0]);

    /*Reservamos el buffer. Uno mas del tamano maximo para poner un byte 0 al final si hace falta.*/
    buffer = (char *) malloc((MAX_SCRIPT_OUTPUT+1) * sizeof(char));
    if(buffer==NULL){
        end = 1;
    }

    /*Inicializamos la cadena*/
    memset(buffer, 0, MAX_SCRIPT_OUTPUT+1);

    /*Enviamos los parametros de entrada, si los hubiese*/
    if(!end && input != NULL){
        /*Escribimos la cadena de entrada*/
        if(write(wpipe[1], input, strlen(input)) < 0){
            end = 1;
            strcpy(buffer, "Error while sending parameters to script.\r\n");
        }    
    }

    close(wpipe[1]); 

    /*Leemos los datos de salida*/
    while (!end){
        /*Ajustamos el descriptor de lectura en el conjunto set*/
        FD_ZERO(&set);
        FD_SET(rpipe[0], &set);
        /*Select espera con timeout*/
        tv.tv_sec = SCRIPT_TIMEOUT;
        tv.tv_usec = 0;
        if(select(FD_SETSIZE, &set, NULL, NULL, &tv) < 0){
            end = 1;
            strcpy(buffer, "Internal server error while running script.\r\n");
            break;
        }
        
        /*Comprobamos si han escrito tras el retorno de select*/
        if(!FD_ISSET(rpipe[0], &set)){
            end = 1;
            strcpy(buffer, "Script output timed out.\r\n");
            break;
        }

        /*Leemos datos.*/
        nread = read(rpipe[0], buffer+total_read, MAX_SCRIPT_OUTPUT-total_read);
        if(nread < 0){
            end = 1;
            strcpy(buffer, "Internal server error while executing script.\r\n");
            break;
        }

        total_read += nread;

        /*Miramos si hay un \r\n*/
        pointer = strstr(buffer, "\r\n");
        if(pointer != NULL){
            /*La cadena finaliza tras el \r\n, es decir, 2 bytes tras pointer*/
            pointer[2] = 0;
            end = 1;
            break;
        }

        /*Miramos si se lleno el buffer*/
        if(total_read >= MAX_SCRIPT_OUTPUT){
            end = 1;
            strcpy(buffer, "Script output too large to process.\r\n");
        }

    }

    /*Terminamos el proceso del script*/
    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);

    close(rpipe[1]);
    close(rpipe[0]);

    return buffer;
}
