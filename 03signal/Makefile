.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=pending sleep signal sendtoparentsignal pending_block_signal sigaction sigqueue main _signal
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
