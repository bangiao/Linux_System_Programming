.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=sigusr_sig
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
