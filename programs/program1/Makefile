# run mingw32-make to run this, i know annoying im sorry
CC = gcc
LIBS = -lpcap
CFLAGS = -g -Wall -pedantic -std=gnu99


PCAPS := $(wildcard *.pcap)
OUTS  := $(PCAPS:.pcap=.out)

all:  trace 
outFiles: $(OUTS)

trace: trace.c checksum.c
	$(CC) $(CFLAGS) -o $@ trace.c checksum.c $(LIBS)

#runs trace on all .pcap files and outputs .out files
%.out: %.pcap
	./trace $< > $@

clean:
	rm -f trace