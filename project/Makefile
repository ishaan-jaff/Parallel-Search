CC=gcc
OMP=-fopenmp -DOMP
CFLAGS=-g -O3 -Wall -std=gnu99

CFILES = run.c cycletimer.c seq-search.c
HFILES = data_structs.h run.h cycletimer.h seq-search.h

dfs-seq: seq-search.c cycletimer.c
	$(CC) $(CFLAGS) -o dfs-seq $(CFILES) $(LDFLAGS)
  
dfs-par: run.c cycletimer.c
	$(CC) $(CFLAGS) -o dfs-par $(CFILES) $(LDFLAGS)
