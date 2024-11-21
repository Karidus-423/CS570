CC = gcc
CFLAGS = -ltirpc
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

test:
	$(CC) -o $(BIN)/sample.oof $(SRC)/procedures.c -lncurses

client: $(CLIENT_OBJ) $(CLIENT_STUB_OBJ) $(XDR_FILTER_OBJ)
	$(CC) -o $(BIN)/client.oof $(CLIENT_OBJ) $(CLIENT_STUB_OBJ) $(XDR_FILTER_OBJ) $(CFLAGS) -lncurses

server: $(SERVER_OBJ) $(SERVER_STUB_OBJ) $(XDR_FILTER_OBJ)
	$(CC) -o $(BIN)/server.oof $(SERVER_OBJ) $(SERVER_STUB_OBJ) $(XDR_FILTER_OBJ) $(CFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(RPC_HEADER)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ)/ssnfs_clnt.o: $(CLIENT_STUB)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ)/ssnfs_svc.o: $(SERVER_STUB)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ)/ssnfs_xdr.o: $(XDR_FILTER)
	$(CC) -c $< -o $@ $(CFLAGS)

$(BIN) $(OBJ):
	mkdir -p $@

clean:
	rm -rf $(OBJ)/*.o $(BIN)/*

