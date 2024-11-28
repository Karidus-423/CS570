CC = gcc
CFLAGS = -lncurses -g

SRC=src
OBJ=obj
BIN=bin
GEN=$(SRC)/gen

SRCS=$(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

CLIENT_OBJS = $(addprefix $(OBJ)/,client.o ssnfs_clnt.o ssnfs_xdr.o)
SERVER_OBJS = $(addprefix $(OBJ)/,server.o ssnfs_svc.o ssnfs_xdr.o srv_utils.o parsing.o)
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

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $< -o $@
	
$(OBJ)/%.o: $(GEN)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGETS) $(OBJS) $(CLIENT_OBJS) $(SERVER_OBJS) $(GEN_FILES) ./database.dt

#Test Utils

server:
	./bin/server-sun

client:
	./bin/client-sun -d localhost

