#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int daemonProcess(){
	pid_t hijoPID;
	int controlError;
	int fd;
	
	/*Fork haciendo que el padre termine*/
	hijoPID = fork();
	if(hijoPID < -1){
		exit(EXIT_FAILURE);
	}else if(hijoPID > 0){
		/*Proceso padre*/
		exit(EXIT_SUCCESS);
	}

	/*Iniciamos nueva sesion*/
	hijoPID = setsid();

	if(hijoPID == -1){
		exit(EXIT_FAILURE);
	}

	/*Fork haciendo que el padre termine*/
	hijoPID = fork();
	if(hijoPID < -1){
		exit(EXIT_FAILURE);
	}else if(hijoPID > 0){
		/*Proceso padre*/
		exit(EXIT_SUCCESS);
	}

	/*Cambiamos al directorio raiz*/
	controlError = chdir("/");
	if(controlError != 0){
		exit(EXIT_FAILURE);
	}

	/*Ponemos la máscara a cero*/
	umask(0);

	/*Redirigimos stdin, stdout y stderr a /dev/null */
	fd = open("/dev/null", O_WRONLY | O_APPEND); 
	if(fd == -1){
		exit(EXIT_FAILURE);
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
}
