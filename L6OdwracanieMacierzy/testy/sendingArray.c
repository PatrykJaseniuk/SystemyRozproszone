#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <mpi.h>

const int MASTER_RANK = 0;
int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int name_len;

int main(int argc, char const *argv[])
{
    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("👋 Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);



    if (world_rank == MASTER_RANK)
    {
        float *array;
        malloc
        MPI_Send(&matrix->nb_lines, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
