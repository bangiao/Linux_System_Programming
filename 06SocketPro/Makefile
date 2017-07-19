.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=01server_1 02client_1 03srvreadline-1 04cltreadline-2 05pro_server 06pro_client 07waitpid_server 08waitpid_client 09shutdown_server 10shutdown_client 11select_base 12shut_select_client 
all:$(BIN)
	
%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@
clean:
	rm -f *.o $(BIN)
