CC=gcc
OMP=-fopenmp -DOMP
CFLAGS=-g -O3 -Wall -std=gnu99

MPI=-DMPI
MPICC = mpicc

CFILES =  cycletimer.c seq-search.c par-search.c main.c util.c data_structs.c
HFILES = data_structs.h cycletimer.h seq-search.h util.h par-search.h cycletimer.h

all: main.c cycletimer.c
	$(CC) $(CFLAGS) -o output $(CFILES) $(LDFLAGS) $(OMP)

dfs: main.c cycletimer.c
	$(CC) $(CFLAGS) -o output $(CFILES) $(LDFLAGS) 

mpi: main.c cycletimer.c
	$(MPICC)  $(MPI) -o output $(CFILES) $(LDFLAGS) -std=gnu99
    