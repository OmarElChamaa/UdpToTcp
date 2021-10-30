CC = gcc -g
CFLAGS = -W -Wall -Wextra -Werror
SOURCES = $(wildcard *.c)
OBJETS = $(SOURCES:.c=.o)
EXEC = main
TEMPS = $(wildcard *.time)

$(EXEC) : $(OBJETS)
	$(CC) $(CFLAGS) -o $@ $^ 
%.o : %.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm $(OBJETS) $(EXEC) $(TEMPS)