
PROG=hfsprocessor
HDFBASEDIR=/home/wselwood/lib/hdf-4.2.10/hdf4/
CC=gcc
CFLAGS=-O2 -Wall -g -rdynamic -std=c11 -I$(HDFBASEDIR)include/
LDFLAGS=-L$(HDFBASEDIR)lib/ -ldl -lm -lmfhdf -ldf -ljpeg -lz

hfsprocessor:
	$(CC) HFSProcessor.c $(CFLAGS) $(LDFLAGS) -o $(PROG)

.PHONY : clean
clean:
	rm -f *.o $(PROG)
