.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=01cond_mutex 02_cond

all:$(BIN)

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

02_cond:02_cond.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

01cond_mutex:01cond_mutex.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
clean:
	rm -f *.o $(BIN)
cleano:
	rm -f *.o
