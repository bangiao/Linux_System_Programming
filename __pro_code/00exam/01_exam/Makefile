.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=signal testMulFork signal_pro
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
