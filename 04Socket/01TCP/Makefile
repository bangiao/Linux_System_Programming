.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=00inet_ntoa_err 01base 02htonl_s_r 03client 03server 04_server 05srvMulfork short_connect_clt
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
