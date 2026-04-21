# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall -std=gnu99
LIBS = 

OBJS = networks.o gethostbyname.o pollLib.o safeUtil.o

all:   cclient server

cclient: cclient.c new.c handletable.c $(OBJS)
	$(CC) $(CFLAGS) -o cclient cclient.c new.c handletable.c handletable.h $(OBJS) $(LIBS)

server: server.c new.c handletable.c $(OBJS)
	$(CC) $(CFLAGS) -o server server.c new.c handletable.c handletable.h $(OBJS) $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f server cclient *.o




