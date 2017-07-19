

.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mulfork 

mulfork: mulfork.o myipc_sem.o myipc_shm.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread
clean:
	rm -f *.o $(BIN)






