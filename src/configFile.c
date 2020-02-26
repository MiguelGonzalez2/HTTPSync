/** *
* Modulo que procesa los ficheros de configuracion.
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 12-02-2020
**/

#include "configFile.h"
#include <stdio.h>
#include <confuse.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>

/*Warnings al usar la librería confuse*/
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"

struct _config_t{
    char *server_root;
    long int max_clients;
    long int listen_port;
    char *server_signature;
    long int daemon_mode;
};

/****
*FUNCIÓN: config_t* ini_config_file();
*ARGS_IN: none
*DESCRIPCION: Inicializa la estructura config_t con los datos parseados del
fichero server_conf.
*ARGS_OUT: config_t* : Estructura con los datos de configuracion.
****/
config_t* ini_config_file(){
    config_t *config;  
    int error;
    cfg_t *cfg;

    config = malloc(sizeof(config_t));
    if(config == NULL){
        return NULL;
    }
 
    config->max_clients = DEFAULT_MAX_CLIENTS;
    config->listen_port = DEFAULT_LISTEN_PORT;
    config->daemon_mode = DEFAULT_DAEMON_MODE;
    config->server_root = malloc(sizeof(char) * (strlen(DEFAULT_SERVER_ROOT)+1));
    strcpy(config->server_root, DEFAULT_SERVER_ROOT);
    config->server_signature = malloc(sizeof(char) * (strlen(DEFAULT_SERVER_SIGNATURE)+1));
    strcpy(config->server_signature, DEFAULT_SERVER_SIGNATURE);

    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("server_root", &(config->server_root)),
        CFG_SIMPLE_STR("server_signature", &(config->server_signature)),
        CFG_SIMPLE_INT("max_clients", &config->max_clients),
        CFG_SIMPLE_INT("listen_port", &config->listen_port),
        CFG_SIMPLE_INT("daemon_mode", &config->daemon_mode),
        CFG_END()
	};

    cfg = cfg_init(opts, 0);

    error = cfg_parse(cfg, "server.conf");
    cfg_free(cfg);

    if(error == CFG_PARSE_ERROR){
        syslog(LOG_ERR,"Error parseando el fichero");
        return config;
    }
    else if(error == CFG_FILE_ERROR){
        syslog(LOG_ERR,"Error parseando el fichero");
        return config;
    }
        
    return config;

}


/****
*FUNCIÓN: void free_config_file(config_t config);
*ARGS_IN: config_t*: Estructura config a liberar
*DESCRIPCION: Libera una estructura config_t
*ARGS_OUT: none
****/
void free_config_file(config_t* config){

    if(config == NULL) return;

    if(config->server_root != NULL){
        free(config->server_root);
        config->server_root = NULL;
    }

    if(config->server_signature != NULL){
        free(config->server_signature);
        config->server_signature = NULL;
    }

    free(config);
    config = NULL;
}

/*
*FUNCIÓN: long int get_config_file_port(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el puerto
*DESCRIPCION: Devuelve el puerto que usa el servidor
*ARGS_OUT: long int: El puerto que usa el servidor
****/
long int get_config_file_port(config_t* config){

    if(config == NULL) return -1;
    return config->listen_port; 
}

/*
*FUNCIÓN: long int get_config_file_max_clients(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el máximo de clientes
*DESCRIPCION: Devuelve el numero maximo de clientes que acepta el servidor
*ARGS_OUT: long int: El numero maximo de clientes
****/
long int get_config_file_maxClients(config_t* config){

    if(config == NULL) return -1;
    return config->max_clients; 
}

/*
*FUNCIÓN: long int get_config_file_daemonMode(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el valor.
*DESCRIPCION: Devuelve 0 si no se ejecutara en demonio, otro valor si si.
*ARGS_OUT: long int: Booleano que indica si es demonio.
****/
long int get_config_file_daemonMode(config_t* config){

    if(config == NULL) return -1;
    return config->daemon_mode; 
}

/*
*FUNCIÓN: char* get_config_file_serverRoot(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el server_root
*DESCRIPCION: Devuelve el directorio root del servidor
*ARGS_OUT: char* : El directorio root del servidor
****/
char* get_config_file_serverRoot(config_t* config){

    if(config == NULL) return NULL;
    return config->server_root; 
}

/*
*FUNCIÓN: char* get_config_file_serverSignature(config_t* config){
*ARGS_IN: config_t*: Estructura config de la que extraemos el nombre del server
*DESCRIPCION: Devuelve el nombre del servidor
*ARGS_OUT: char* : El nombre del servidor
****/
char* get_config_file_serverSignature(config_t* config){

    if(config == NULL) return NULL;
    return config->server_signature; 
}
