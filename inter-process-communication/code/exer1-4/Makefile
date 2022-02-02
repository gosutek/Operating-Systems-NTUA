CC = gcc
CFLAGS = -Werror -O2

all: calc

calc: calc.o tree.o proc-common.o
	$(CC) -o calc calc.o tree.o proc-common.o

calc.o: calc.c
	$(CC) $(CFLAGS) -o calc.o -c calc.c

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -o tree.o -c tree.c

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

clean:
	rm -f calc calc.o tree.o proc-common.o
