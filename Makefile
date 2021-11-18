vpath %.h 

CC = gcc -g
CFLAGS = -W -Wall -Wextra -Werror

exe: source destination
source: source.o 

destination : server.o


source.o:source.c packet.h
	$(CC) $(CFLAGS) -c source.c
server.o:server.c packet.h
	$(CC) $(CFLAGS) -c server.c
clean:
	rm *.o source destination
	@echo Clean!
plot :
	gnuplot StopWaitFig.p