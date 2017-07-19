.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=sigaction sigqueue
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
