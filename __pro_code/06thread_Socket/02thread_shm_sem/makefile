.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=03srv 04cli
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread
	
04cli:04cli.o comsock.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
	
03srv:03srv.o comsock.o myipc_sem.o myipc_shm.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

clean:
	rm -f *.o $(BIN)




