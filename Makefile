CC = gcc
CFLAGS = -Wall -Wextra

minigit: main.c fila.c fila.h
	$(CC) $(CFLAGS) -o minigit main.c fila.c

clean:
	rm -f minigit minigit.exe
