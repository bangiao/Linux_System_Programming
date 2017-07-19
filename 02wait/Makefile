.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mywait mul_wait_fork
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
