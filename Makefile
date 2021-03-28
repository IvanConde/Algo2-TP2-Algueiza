CC = gcc
CFLAGS = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes
EXEC = algueiza
OBJS = strutil.c strutil.h hash.c hash.h abb.c abb.h pila.c pila.h heap.c heap.h cola.c cola.h

all: build
	./$(EXEC) < comandos.txt

run: $(EXEC)
	./$(EXEC) < comandos.txt

build: algueiza.c $(OBJS)
	$(CC) $(CFLAGS) algueiza.c $(OBJS) -o $(EXEC)

valgrind: build
	valgrind $(VFLAGS) ./$(EXEC) < comandos.txt

clean:
	rm -f *.o $(EXEC)