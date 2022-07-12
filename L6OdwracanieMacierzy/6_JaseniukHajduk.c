// Autorzy: Patryk Jaseniuk; Franciszek Hajduk @ Konto: PR1g1
// Cwiczenie: 2, Data opracowania: 2022-07-05, Wersja: 2

// import libraries
#include <mpi.h>
#include <stdio.h>
#include "Matrix.h"

const int MASTER_RANK = 0;
int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int name_len;

void handelError(int result)
{
    // get error message
    char error_message[MPI_MAX_ERROR_STRING];
    int error_string_length;
    MPI_Error_string(result, error_message, &error_string_length);
    printf("Error in processor:%s, rank: %d\n message: %s\n", processor_name, world_rank, error_message);
    MPI_Abort(MPI_COMM_WORLD, 1);
}
int main(int argC, char **args)
{
    // Initialize the MPI environment
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

    struct Matrix matrix;

    if (world_rank == MASTER_RANK)
    {
        // check if there is only one argument
        if (argC != 2)
        {
            printf("🐛 Wrong number of arguments!\n");
            return 1;
        }

        // open file
        FILE *file = fopen(args[1], "r");
        if (file == NULL)
        {
            printf("🐛 Cannot open file!\n");
            return 1;
        }

        // check corectness of file

        // take two numbers form first line and check if they are correclt
        int n, m;
        fscanf(file, "%d %d", &n, &m);
        if (n != m)
        {
            printf("🐛 Matrix is not square!\n");
            return 1;
        }

        // check if matrix is not too big
        if (n > 100)
        {
            printf("🐛 Matrix is too big!\n");
            return 1;
        }

        int quantity = n * n;
        // check if there is equal quantity of numbers in file
        int i = 0;
        int number;
        while (fscanf(file, "%d", &number) != EOF)
        {
            i++;
        }
        if (i != quantity)
        {
            printf("🐛 Wrong quantity of numbers!\n");
            return 1;
        }

        // set readnig from second line
        fseek(file, 3, SEEK_SET);

        allocateMatrix(&matrix, n, n);
        // take matrix from file
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                float value;
                fscanf(file, "%f", &value);
                setValue(&matrix, i, j, value);
            }
        }
        // close file
        fclose(file);

        // broadcast matrix to all processes
        MPI_Bcast(&matrix, sizeof(matrix), MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
        MPI_Bcast(matrix.data, matrix.nb_columns * matrix.nb_lines, MPI_FLOAT, MASTER_RANK, MPI_COMM_WORLD);
    }
    else
    {
        // print size of matrix
        printf("sieze of matrix: %ld\n", sizeof(matrix));

        // recive matrix from master
        MPI_Bcast(&matrix, sizeof(matrix), MPI_BYTE, MASTER_RANK, MPI_COMM_WORLD);
        allocateMatrix(&matrix, matrix.nb_columns, matrix.nb_lines);
        MPI_Bcast(matrix.data, matrix.nb_columns * matrix.nb_lines, MPI_FLOAT, MASTER_RANK, MPI_COMM_WORLD);
    }

    // print: 'matrix':
    printf("Macierz:\n");
    printMatrix(&matrix);

    struct Matrix inversMatrix;
    allocateMatrix(&inversMatrix, matrix.nb_columns, matrix.nb_columns);
    makeIdentity(&inversMatrix);

    // print 'inversMatrix':
    printf("Macierz jednostkowa:\n");
    printMatrix(&inversMatrix);

    gaussJordanElimination(&matrix, &inversMatrix);

    if (world_rank == MASTER_RANK)
    {
        // print 'inversMatrix':
        printf("Macierz odwrotna:\n");
        printMatrix(&inversMatrix);

        // checking for correctness of inversMatrix:
        if (checking(&matrix, &inversMatrix))
        {
            printf("👌 Macierz odwrotna jest poprawna!\n");
        }
        else
        {
            printf("🐛 Macierz odwrotna jest niepoprawna!\n");
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}