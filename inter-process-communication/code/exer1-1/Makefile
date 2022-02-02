CC = gcc
CFLAGS = -Wall -O2

all: pstree1_1

pstree1_1: pstree1_1.o proc-common.o
	$(CC) -o pstree1_1 pstree1_1.o proc-common.o

proc-common.o: proc-common.c proc-common.h
	$(CC) $(CFLAGS) -o proc-common.o -c proc-common.c

pstree1_1.o: pstree1_1.c proc-common.h
	$(CC) $(CFLAGS) -o pstree1_1.o -c pstree1_1.c

clean:
	rm -f pstree1_1 proc-common.o pstree1_1.o
