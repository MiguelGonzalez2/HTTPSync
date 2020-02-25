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

#endif
