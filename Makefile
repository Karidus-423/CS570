CC = gcc
CFLAGS = -lncurses -g -l/usr/include/tirpc -ltirpc

SRC=src
OBJ=obj
BIN=bin
GEN=$(SRC)/gen
CLNT=$(SRC)/client
SRV=$(SRC)/server

SRCS=$(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

CLIENT_OBJS = $(addprefix $(OBJ)/,client.o ssnfs_clnt.o ssnfs_xdr.o)
SERVER_OBJS = $(addprefix $(OBJ)/,server.o ssnfs_svc.o ssnfs_xdr.o srv_utils.o parsing.o)

SRV_FILE = $(addprefix $(SRV)/, *.c)
CLNT_FILES = $(addprefix $(CLNT)/,*.c)
GEN_FILES = $(addprefix $(GEN)/, *.c ssnfs.h)

TARGETS = $(BIN)/client-sun $(BIN)/server-sun


all: gen $(OBJ) $(BIN) $(TARGETS)

gen: 
	cd $(GEN) && rpcgen ssnfs.x

$(OBJ) $(BIN):
	mkdir -p $@

$(BIN)/client-sun: $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(BIN)/server-sun: $(SERVER_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ)/%.o: $(SRV)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

	
$(OBJ)/%.o: $(CLNT)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@ $(CFLAGS)


$(OBJ)/%.o: $(GEN)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o $(TARGETS) $(OBJS) $(CLIENT_OBJS) $(SERVER_OBJS) $(GEN_FILES) ./database.dt

#Test Utils

server:
	./bin/server-sun

client:$(BIN)/client-sun
	./bin/client-sun localhost

