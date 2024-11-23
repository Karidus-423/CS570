CC = gcc
CFLAGS = -lncurses

TARGETS = client-sun server-sun

CLIENT_OBJS = client.o ssnfs_clnt.o ssnfs_xdr.o
SERVER_OBJS = server.o ssnfs_svc.o ssnfs_xdr.o

all: $(TARGETS)

client-sun: $(CLIENT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

server-sun: $(SERVER_OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o $(TARGETS)

