

.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mulforkshm 

all:$(BIN)

mulforkshm: mulforkshm.o  myipc_sem.o myipc_shm.o
	$(CC) $(CFLAGS) $^ -o $@  

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)






