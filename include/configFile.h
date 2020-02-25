/**
*
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 12-02-2020

**/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#define DEFAULT_MAX_CLIENTS 10 
#define DEFAULT_LISTEN_PORT 8080 
#define DEFAULT_SERVER_ROOT "htmlfiles/"
#define DEFAULT_SERVER_SIGNATURE "ServerRedes"

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
*FUNCIÓN: long int get_config_file_port(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el puerto
*DESCRIPCION: Devuelve el puerto que usa el servidor
*ARGS_OUT: long int: El puerto que usa el servidor
****/
long int get_config_file_port(config_t* config);

/*
*FUNCIÓN: long int get_config_file_max_clients(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el máximo de clientes
*DESCRIPCION: Devuelve el numero maximo de clientes que acepta el servidor
*ARGS_OUT: long int: El numero maximo de clientes
****/
long int get_config_file_maxClients(config_t* config);

/*
*FUNCIÓN: char* get_config_file_serverRoot(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el server_root
*DESCRIPCION: Devuelve el directorio root del servidor
*ARGS_OUT: char* : El directorio root del servidor
****/
char* get_config_file_serverRoot(config_t* config);

/*
*FUNCIÓN: char* get_config_file_serverSignature(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el nombre del server
*DESCRIPCION: Devuelve el nombre del servidor
*ARGS_OUT: char* : El nombre del servidor
****/
char* get_config_file_serverSignature(config_t* config);

#endif
