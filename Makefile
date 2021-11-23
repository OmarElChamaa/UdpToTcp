vpath %.h 

CC = gcc -g
CFLAGS = -W -Wall -Wextra -Werror

all: source server

source:source.c packet.h
	$(CC) $(CFLAGS) -o $@ $^ -lm
server:server.c packet.h
	$(CC) $(CFLAGS) -o $@ $^ 
clean:
	rm source server
	@echo Clean!
plot :
	gnuplot StopWaitFig.p
