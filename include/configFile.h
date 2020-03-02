/**
* Modulo que procesa los ficheros de configuracion.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 12-02-2020

**/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

/*Valores por defecto para los campos del fichero*/
#define DEFAULT_MAX_CLIENTS 10 
#define DEFAULT_LISTEN_PORT 8080 
#define DEFAULT_SERVER_ROOT "htmlfiles/"
#define DEFAULT_SERVER_SIGNATURE "ServerRedes"
#define DEFAULT_DAEMON_MODE 1
#define DEFAULT_LISTEN_QUEUE 5 /*!<Cola de espera para conexiones no aceptadas*/
#define DEFAULT_READ_TIMEOUT 10 /*!<Temporizador para operaciones de lectura*/


typedef struct _config_t config_t;

/****
*FUNCIÓN: config_t* ini_config_file();
*ARGS_IN: none
*DESCRIPCION: Inicializa la estructura config_t con los datos parseados del
fichero server_conf.
*ARGS_OUT: config_t* : Estructura con los datos de configuracion.
****/
config_t* ini_config_file();

/****
*FUNCIÓN: void free_config_file(config_t config);
*ARGS_IN: config_t*: Estructura config a liberar 
*DESCRIPCION: Libera una estructura config_t
*ARGS_OUT: none
****/
void free_config_file(config_t* config);

/*
*FUNCIÓN: long int get_config_file_port(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el puerto
*DESCRIPCION: Devuelve el puerto que usa el servidor
*ARGS_OUT: long int: El puerto que usa el servidor
****/
long int get_config_file_port(config_t* config);

/*
*FUNCIÓN: long int get_config_file_max_clients(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el máximo de clientes
*DESCRIPCION: Devuelve el numero maximo de clientes que acepta el servidor
*ARGS_OUT: long int: El numero maximo de clientes
****/
long int get_config_file_maxClients(config_t* config);

/*
*FUNCIÓN: char* get_config_file_serverRoot(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el server_root
*DESCRIPCION: Devuelve el directorio root del servidor
*ARGS_OUT: char* : El directorio root del servidor
****/
char* get_config_file_serverRoot(config_t* config);

/*
*FUNCIÓN: char* get_config_file_serverSignature(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el nombre del server
*DESCRIPCION: Devuelve el nombre del servidor
*ARGS_OUT: char* : El nombre del servidor
****/
char* get_config_file_serverSignature(config_t* config);

/*
*FUNCIÓN: long int get_config_file_daemonMode(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el valor.
*DESCRIPCION: Devuelve 0 si no se ejecutara en demonio, otro valor si si.
*ARGS_OUT: long int: Booleano que indica si es demonio.
****/
long int get_config_file_daemonMode(config_t* config);

/*
*FUNCION: long int get_config_file_listenQueue(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el valor listen_queue
*DESCRIPCION: Devuelve el tamanio de la cola de espera para conexiones
*no aceptadas
*ARGS_OUT: char* : El tamaño de la listen queue
****/
long int get_config_file_listenQueue(config_t* config);

/*
*FUNCION: long int get_config_file_readTimeout(config_t* config)
*ARGS_IN: config_t*: Estructura config de la que extraemos el timeout de lectura
*DESCRIPCION: Devuelve el timeout para operaciones de lectura
*ARGS_OUT: char* : El timeout para operaciones de lectura
****/
long int get_config_file_readTimeout(config_t* config);


#endif
