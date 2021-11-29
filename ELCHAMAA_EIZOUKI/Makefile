vpath %.h 

CC = gcc -g
CFLAGS = -W -Wall -Wextra -Werror

all: source server

source:src/source.c inc/packet.h
	$(CC) $(CFLAGS) -o $@ $^ -lm
server:src/server.c inc/packet.h
	$(CC) $(CFLAGS) -o $@ $^ 
clean:
	rm source server
	@echo Clean!
plot :
	gnuplot bin/StopWaitFig.p
