#
# Makefile
# 

CC = gcc

CFLAGS = -Wall -O2 -pthread
LIBS =

all: mandel-sem mandel-buffer


## Mandel

mandel-sem: mandel-lib.o mandel-sem.o
	$(CC) $(CFLAGS) -o mandel-sem mandel-lib.o mandel-sem.o $(LIBS)

mandel-buffer: mandel-lib.o mandel-buffer.o
	$(CC) $(CFLAGS) -o mandel-buffer mandel-lib.o mandel-buffer.o $(LIBS)

mandel-lib.o: mandel-lib.h mandel-lib.c
	$(CC) $(CFLAGS) -c -o mandel-lib.o mandel-lib.c $(LIBS)

mandel-sem.o: mandel-sem.c
	$(CC) $(CFLAGS) -c -o mandel-sem.o mandel-sem.c $(LIBS)

mandel-buffer.o: mandel-buffer.c
	$(CC) $(CFLAGS) -c -o mandel-buffer.o mandel-buffer.c $(LIBS)

clean: 
	rm -f *.o mandel-sem mandel-buffer
