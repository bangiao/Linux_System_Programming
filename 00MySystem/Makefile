.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=mysystem
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
