CC = gcc
SRC = src
OBJ = obj
BIN = bin

CLIENT_STUB = $(SRC)/genrpc/ssnfs_clnt.c
SERVER_STUB = $(SRC)/genrpc/ssnfs_svc.c
XDR_FILTER = $(SRC)/genrpc/ssnfs_xdr.c
RPC_HEADER = $(SRC)/genrpc/ssnfs.h

CLIENT_OBJ = $(OBJ)/client.o
CLIENT_STUB_OBJ = $(OBJ)/ssnfs_clnt.o
SERVER_OBJ = $(OBJ)/server.o
SERVER_STUB_OBJ = $(OBJ)/ssnfs_svc.o
XDR_FILTER_OBJ = $(OBJ)/ssnfs_xdr.o

all: $(BIN) $(OBJ) client server

client: $(CLIENT_OBJ) $(CLIENT_STUB_OBJ) $(XDR_FILTER_OBJ)
	$(CC) -o $(BIN)/sun-client $(CLIENT_OBJ) $(CLIENT_STUB_OBJ) $(XDR_FILTER_OBJ) -ltirpc

server: $(SERVER_OBJ) $(SERVER_STUB_OBJ) $(XDR_FILTER_OBJ)
	$(CC) -o $(BIN)/sun-server $(SERVER_OBJ) $(SERVER_STUB_OBJ) $(XDR_FILTER_OBJ) -ltirpc

$(OBJ)/%.o: $(SRC)/%.c $(RPC_HEADER)
	$(CC) -c $< -o $@ -I/usr/include/tirpc

$(OBJ)/ssnfs_clnt.o: $(CLIENT_STUB)
	$(CC) -c $< -o $@ -I/usr/include/tirpc

$(OBJ)/ssnfs_svc.o: $(SERVER_STUB)
	$(CC) -c $< -o $@ -I/usr/include/tirpc

$(OBJ)/ssnfs_xdr.o: $(XDR_FILTER)
	$(CC) -c $< -o $@ -I/usr/include/tirpc

$(BIN) $(OBJ):
	mkdir -p $@

clean:
	rm -rf $(OBJ)/*.o $(BIN)/*

