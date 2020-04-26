#include "mpi.h"
#include <stdio.h>




// To compile mpi file mpicc -O2 -g -Wall mpi1Test.c -o hello
// To run mpi file mpirun -np 2 ./hello
int main( int argc, char *argv[] )
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    printf("Hello! I am %d of %d\n", rank, size );
    outmsg("PLACKA\n");
    MPI_Finalize();
    return 0;
}

