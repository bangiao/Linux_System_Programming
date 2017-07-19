

.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN= fifow fifor dm01_fifo
all:$(BIN)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(BIN)





