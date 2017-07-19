.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=pro_signal signal_all signalpro signal_recv newsignal
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
