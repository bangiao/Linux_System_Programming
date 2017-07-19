.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=01client 02server
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
