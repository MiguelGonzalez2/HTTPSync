/**
* Modulo que gestiona las respuestas http.
*
* @author Miguel Gonzalez, Alejandro Bravo
* @version 1.0
* @date 06-02-2020
*/

#ifndef HTTP_REPLY_H
#define HTTP_REPLY_H

#include "http_request.h"

/****
* FUNCIÓN: int http_reply_send(int conn_fd, request_t req)
* ARGS_IN: int conn_fd: Descriptor del socket al que enviar la reply.
*          request_t req: Peticion a la que responder.
* DESCRIPCIÓN: Recibe una peticion y responde segun lo solicitado.
* ARGS_OUT: int - Devuelve el numero de bytes transferidos con exito.
****/
int http_reply_send(int conn_fd, request_t req);
#endif
