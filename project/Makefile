CC=gcc
OMP=-fopenmp -DOMP
CFLAGS=-g -O3 -Wall -std=gnu99

CFILES =  cycletimer.c seq-search.c par-search.c main.c util.c data_structs.c
HFILES = data_structs.h cycletimer.h seq-search.h util.h par-search.h cycletimer.h

all: main.c cycletimer.c
	$(CC) $(CFLAGS) -o output $(CFILES) $(LDFLAGS)

