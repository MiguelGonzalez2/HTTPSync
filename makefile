#####MAKEFILE PRACTICA 1. AUTORES. MIGUEL GONZALEZ Y ALEJANDRO BRAVO#####
#####Llamar a make genera el server y limpia los objetos generados  #####

INC = ./include/
SRC = ./src/
LIB = ./lib/
SRCLIB = ./srclib/
BIN = ./bin/
BINLIB = ./binlib/
EXE = server

flags = -Wall -ansi -pedantic -I$(INC) -L$(LIB)
CC = gcc

all: $(EXE) clean

$(BIN):
	mkdir $(BIN)

$(LIB):
	mkdir $(LIB)

$(BINLIB):
	mkdir $(BINLIB)
	
#Libreria de sockets.
$(LIB)libsocket.a: $(BINLIB)libsocket.o $(LIB)
	ar rcs $(LIB)libsocket.a $<

#Libreria de threads.
$(LIB)libPoolThreads.a: $(BINLIB)libPoolThreads.o $(LIB)
	ar rcs $(LIB)libPoolThreads.a $<

#Libreria de picohttpparser
$(LIB)libPicohttpparser.a: $(BINLIB)libPicohttpparser.o $(LIB)
	ar rcs $(LIB)libPicohttpparser.a $<

#Ejecutable
$(EXE): $(LIB)libsocket.a $(LIB)libPoolThreads.a $(LIB)libPicohttpparser.a $(BIN)daemon.o $(BIN)server.o $(BIN)http_request.o $(BIN)http_reply.o
	$(CC) $(flags) -o $(EXE) $(BIN)server.o $(BIN)daemon.o $(BIN)http_request.o $(BIN)http_reply.o -lsocket -lPoolThreads -lpthread -lPicohttpparser

#Regla para compilar objetos de libreria.
$(BINLIB)%.o:: $(SRCLIB)%.c $(INC)%.h $(BINLIB)
	$(CC) $(flags) -c -o $@ $< -lpthread -pthread 

#Regla para compilar objetos.
$(BIN)%.o:: $(SRC)%.c $(LIB)libPicohttpparser.a $(BIN)
	$(CC) $(flags) -c -o $@ $< -lpthread -pthread -lPicohttpparser
	
#Clean
clean:
	rm -rf $(BIN) $(BINLIB) $(LIB)*
