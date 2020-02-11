/**
*Cabecera de la funcion auxiliar de demonizacion.
*
*@author Alejandro Bravo, Miguel Gonzalez
*@version 1.0
*@date 08-02-2020
**/
#ifndef DAEMON_H
#define DAEMON_H
/***
*FUNCION: int daemon()
*ARGS_IN: none
*DESCRIPCION: Convierte el proceso en un daemon, pasando a segundo plano
y dejando de ser interactivo.
*ARGS_OUT: Devuelve EXIT_SUCCESS si funciono correctamente
y EXIT_FAILURE en caso de que algo falle.
***/
int daemonProcess();
#endif
