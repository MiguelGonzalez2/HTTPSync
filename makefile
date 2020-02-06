INC = include/
SRC = src/
LIB = lib/
SRCLIB = srclib/
BIN = bin/
BINLIB = binlib/

flags = -Wall -ansi -pedantic -I$(INC)
CC = gcc

#Regla para compilar objetos.
$(BIN)%.o: $(SRC)%.c $(INC)%.h
	$(CC) $(flags) -c -o $@ $<
	
#Regla para compilar objetos de libreria.
$(BINLIB)%.o: $(SRCLIB)%.c $(INC)%.h
	$(CC) $(flags) -c -o $@ $<
	
#Libreria de sockets.
$(LIB)libsocket.a: $(BINLIB)libsocket.o
	ar rcs libsocket.a $(BINLIB)libsocket.objetos
	
#Clean
clean:
	rm -rf $(BIN) $(BINLIB)