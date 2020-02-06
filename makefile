INC = include/
SRC = src/
LIB = lib/
SRCLIB = srclib/
BIN = bin/
BINLIB = binlib/

flags = -Wall -ansi -pedantic -I$(INC)
CC = gcc

$(BIN):
	mkdir $(BIN)

$(BINLIB):
	mkdir $(BINLIB)

$(LIB):
	mkdir $(LIB)

#Regla para compilar objetos.
$(BIN)%.o: $(SRC)%.c $(INC)%.h $(BIN)
	$(CC) $(flags) -c -o $@ $<
	
#Regla para compilar objetos de libreria.
$(BINLIB)%.o: $(SRCLIB)%.c $(INC)%.h $(BINLIB)
	$(CC) $(flags) -c -o $@ $<
	
#Libreria de sockets.
$(LIB)libsocket.a: $(BINLIB)libsocket.o $(LIB)
	ar rcs $(LIB)libsocket.a $<
	
#Clean
clean:
	rm -rf $(BIN) $(BINLIB)
