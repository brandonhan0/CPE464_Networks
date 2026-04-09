PCAPS := $(wildcard *.pcap)
OUTS  := $(PCAPS:.pcap=.out)
LIBS = -lpcap
CC = gcc

all:  trace 
outFiles: $(OUTS)

trace: trace.c checksum.c
	$(CC) $(CFLAGS) -o $@ trace.c checksum.c $(LIBS)

#runs trace on all .pcap files and outputs .out files
%.out: %.pcap
	./trace $< > $@

clean:
	rm -f trace
