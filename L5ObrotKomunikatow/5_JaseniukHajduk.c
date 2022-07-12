// Autorzy: Patryk Jaseniuk ; Franciszek Hajduk; @ Konto: PR1g3 - (dostep z konta PR1g2)
// Cwiczenie: 4, Data opracowania: 2022-07-11

// mpi library
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int MASTER_RANK = 0;
const int SLAVE_RANK = 1;

int main(int argc, char *argv[])
{
    int quantityOfMesagees = 0;
    int quantityOfRaports = 10;

    // get quantity of messages form arguments
    if (argc > 1)
    {
        quantityOfMesagees = atoi(argv[1]);
    }
    else
    {
        printf("Nie podano liczby wiadomosci.\n");
        return 1;
    }

    // get quantity of raports form arguments
    if (argc > 2)
    {
        quantityOfRaports = atoi(argv[2]);
    }
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    if (world_size < 2)
    {
        printf("Za malo procesow.\n");
        return 20;
    }

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    char message = 0xFF;
    int result;
    void abort(int result)
    {
        // get error message
        char error_message[MPI_MAX_ERROR_STRING];
        int error_string_length;
        MPI_Error_string(result, error_message, &error_string_length);
        printf("Error in processor:%s, rank: %d\n message: %s\n", processor_name, world_rank, error_message);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (world_rank == MASTER_RANK)
    {
        double timeBegin = MPI_Wtime();

        for (int i = 0; i < quantityOfMesagees; i++)
        {
            
            double timeBeginSend = MPI_Wtime();
            // sedn is blocked until message is received
            result = MPI_Send(&message, 1, MPI_CHAR, SLAVE_RANK, 0, MPI_COMM_WORLD);
            if (result != MPI_SUCCESS)
            {
                abort(result);
                return 00;
            }

            result = MPI_Recv(&message, 1, MPI_CHAR, SLAVE_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (result != MPI_SUCCESS)
            {
                abort(result);
                return 01;
            }
            double timeEndSend = MPI_Wtime();
            if (quantityOfRaports > 0)
            {
                printf("Czas trwania obrotu wiadomosci nr: %d -->%d[ns]\n", i, (int)((timeEndSend - timeBeginSend) * 1000000000));
                quantityOfRaports--;
            }

            // print time without decimal
        }
        double timeEnd = MPI_Wtime();
        double timeDuration = timeEnd - timeBegin;

        printf("-----------------------------------------------------\n");
        printf("Liczba wiadomosci: %d\n", quantityOfMesagees);
        // print time duration with 3 decimal places
        printf("Łączny czas: %.3f[s]\n", timeDuration);
        printf("Sredni czas przesylania w jednym cyklu: %.0f[ns]\n", (timeDuration / quantityOfMesagees * 1000000000));
        printf("Sredni czas przesylania w jedna strone: %.0f[ns]\n", (timeDuration / quantityOfMesagees * 1000000000 / 2));
    }

    if (world_rank == SLAVE_RANK)
    {
        for (int i = 0; i < quantityOfMesagees; i++)
        {
            result =MPI_Recv(&message, 1, MPI_CHAR, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(result != MPI_SUCCESS)
            {
                abort(result);
                return 10;//copillot generate return value by it self
            }
            MPI_Send(&message, 1, MPI_CHAR, MASTER_RANK, 0, MPI_COMM_WORLD);
            if(result != MPI_SUCCESS)
            {
                abort(result);
                return 11;
            }
        }
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}