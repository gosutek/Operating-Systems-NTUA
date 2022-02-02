CC = gcc
CFLAGS = -Wall -O2

all: abspstree

abspstree: abspstree.o tree.o proc-common.o
	$(CC) -o abspstree abspstree.o tree.o proc-common.o

abspstree.o: abspstree.c tree.h proc-common.h
	$(CC) $(CFLAGS) -o abspstree.o -c abspstree.c

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -o tree.o -c tree.c

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

clean:
	rm -f abspstree abspstree.o tree.o proc-common.o
