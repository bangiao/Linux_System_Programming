.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=pending_kill kill
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
