// Autorzy: Patryk Jaseniuk; Franciszek Hajduk @ Konto: PR1g1
// Cwiczenie: 2, Data opracowania: 2022-07-05, Wersja: 2

// import libraries
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
// #include "Matrix.h"

int const MASTER_RANK;
int world_size;
int world_rank;
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

struct Matrix
{
    int nb_lines;
    int nb_columns;
    float *data;
};

void allocateMatrix(struct Matrix *m, int nb_lines, int nb_columns)
{
    m->nb_lines = nb_lines;
    m->nb_columns = nb_columns;
    m->data = (float *)malloc(nb_lines * nb_columns * sizeof(float));
}

float getValue(struct Matrix *m, int line, int column)
{
    return m->data[line * m->nb_columns + column];
}

void setValue(struct Matrix *m, int line, int column, float value)
{
    m->data[line * m->nb_columns + column] = value;
}

void fillMatrixWithAscendingValues(struct Matrix *m)
{
    int i, j;
    for (i = 0; i < m->nb_lines; i++)
    {
        for (j = 0; j < m->nb_columns; j++)
        {
            setValue(m, i, j, i * m->nb_columns + j + 1);
        }
    }
}

void makeIdentity(struct Matrix *m)
{
    int i, j;
    for (i = 0; i < m->nb_lines; i++)
    {
        for (j = 0; j < m->nb_columns; j++)
        {
            if (i == j)
            {
                setValue(m, i, j, 1);
            }
            else
            {
                setValue(m, i, j, 0);
            }
        }
    }
}

void multiplyRowByScalar(struct Matrix *m, int row, float scalar)
{
    int i;
    for (i = 0; i < m->nb_columns; i++)
    {
        setValue(m, row, i, getValue(m, row, i) * scalar);
    }
}

void multiplyRowByScalarAndAddToRow(struct Matrix *m, int row, float scalar, int row_to_add)
{
    int i;
    for (i = 0; i < m->nb_columns; i++)
    {
        setValue(m, row, i, (getValue(m, row, i) + getValue(m, row_to_add, i) * scalar));
    }
}

void printMatrix(struct Matrix *m)
{
    //    print with 2 decimals and tabulation
    int i, j;
    for (i = 0; i < m->nb_lines; i++)
    {
        for (j = 0; j < m->nb_columns; j++)
        {
            printf("%.2f\t", getValue(m, i, j));
        }
        printf("\n");
    }
}

char isSingular = 0;

// void zeroKolumnBelowDiagonal(struct Matrix *m, struct Matrix *result, int column)
// {
//     // make diagonal 1
//     float diagonal = getValue(m, column, column);
//     if (diagonal <= 1e-6 && diagonal >= -1e-6)
//     {
//         isSingular = 1;
//         return;
//     }
//     multiplyRowByScalar(m, column, 1.0 / diagonal);
//     multiplyRowByScalar(result, column, 1.0 / diagonal);

//     // tutaj zrownolegle
//     // wyliczanie liczby

//     for (int row = column + 1; row < m->nb_lines; row++)
//     {
//         float factor = getValue(m, row, column);
//         multiplyRowByScalarAndAddToRow(m, row, -factor, column);
//         multiplyRowByScalarAndAddToRow(result, row, -factor, column);
//     }
// }

// void zeroLowerTriangle(struct Matrix *m, struct Matrix *result)
// {
//     for (int column = 0; column < m->nb_columns; column++)
//     {
//         zeroKolumnBelowDiagonal(m, result, column);
//         if (isSingular)
//         {
//             return;
//         }
//     }
// }
// void zeroKolumnAboveDiagonal(struct Matrix *m, struct Matrix *result, int column)
// {
//     for (int row = column - 1; row >= 0; row--)
//     {
//         float factor = getValue(m, row, column);
//         multiplyRowByScalarAndAddToRow(m, row, -factor, column);
//         multiplyRowByScalarAndAddToRow(result, row, -factor, column);
//     }
// }
// void zeroUpperTriangle(struct Matrix *m, struct Matrix *result)
// {
//     for (int coumn = m->nb_columns - 1; coumn >= 0; coumn--)
//     {
//         zeroKolumnAboveDiagonal(m, result, coumn);
//     }
// }

int gaussJordanElimination(struct Matrix *matrix, struct Matrix *inversMatrix)
{
    for (int column = 0; column < matrix->nb_columns; column++)
    {
        if (world_rank == MASTER_RANK)
        {
            // make diagonal 1
            float diagonal = getValue(matrix, column, column);
            if (diagonal <= 1e-6 && diagonal >= -1e-6)
            {
                printf("Matrix is singular\n");
                return 1;
            }
            multiplyRowByScalar(matrix, column, 1.0 / diagonal);
            multiplyRowByScalar(inversMatrix, column, 1.0 / diagonal);
        }

        // tutaj zrownolegle
        // wyliczanie liczby linin na proces im nizej tym mnije lini zostalo
        int pozostaleWiersze = matrix->nb_lines - column - 1;
        int linesPerProcess = pozostaleWiersze / world_size;
        int resztaLini = pozostaleWiersze % world_size;
        int startingLine;
        int endingLine;

        if (world_rank == MASTER_RANK)
        {
            startingLine = column + 1;
            endingLine = startingLine + linesPerProcess + resztaLini;
            printf("linesPerProcessMaster: %d\n", linesPerProcess);
        }
        else
        {
            printf("linesPerPoroces: %d\n", linesPerProcess);
            startingLine = column + 1 + resztaLini + world_rank * linesPerProcess;
            endingLine = startingLine + linesPerProcess;
        }

        for (int row = startingLine; row < endingLine; row++)
        {
            float factor = getValue(matrix, row, column);
            multiplyRowByScalarAndAddToRow(matrix, row, -factor, column);
            multiplyRowByScalarAndAddToRow(inversMatrix, row, -factor, column);
        }
        // all proceses send its part to master, master assembly it and broadcast complit matrixes to all processes
        printf("world_rank: %d\n", world_rank);
        printf("MASTER_RANK: %d\n", MASTER_RANK);
        if (world_rank == MASTER_RANK)
        {
            // assemble lines from all processes
            for (int i = 1; i < world_size; i++)
            {
                float *wskNaMiejsceGdzieMajaBycZapisaneDane = matrix->data + ((column + resztaLini + 1 + linesPerProcess * i) * matrix->nb_columns);
                MPI_Recv(wskNaMiejsceGdzieMajaBycZapisaneDane, linesPerProcess * matrix->nb_columns, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                wskNaMiejsceGdzieMajaBycZapisaneDane = inversMatrix->data + ((column + resztaLini + 1 + linesPerProcess * i) * matrix->nb_columns);
                MPI_Recv(wskNaMiejsceGdzieMajaBycZapisaneDane, linesPerProcess * matrix->nb_columns, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        else
        {
            printf("sending data to master\n");
            // send lines of proces to master
            float *wskaznikNaPoczatekDanychDoWyslania = matrix->data + ((column + resztaLini + 1 + linesPerProcess * world_rank) * matrix->nb_columns);
            MPI_Send(wskaznikNaPoczatekDanychDoWyslania, linesPerProcess * matrix->nb_columns, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD);
            wskaznikNaPoczatekDanychDoWyslania = inversMatrix->data + ((column + resztaLini + 1 + linesPerProcess * world_rank) * matrix->nb_columns);
            MPI_Send(wskaznikNaPoczatekDanychDoWyslania, linesPerProcess * matrix->nb_columns, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD);
        }
    }
    for (int column = matrix->nb_columns - 1; column >= 0; column--)
    {

        for (int row = column - 1; row >= 0; row--)
        {
            float factor = getValue(matrix, row, column);
            multiplyRowByScalarAndAddToRow(matrix, row, -factor, column);
            multiplyRowByScalarAndAddToRow(inversMatrix, row, -factor, column);
        }
    }
    return 0;
}

struct Matrix multiplyMatrix(struct Matrix *m1, struct Matrix *m2)
{
    struct Matrix result;
    allocateMatrix(&result, m1->nb_lines, m2->nb_columns);
    int i, j, k;
    for (i = 0; i < m1->nb_lines; i++)
    {
        for (j = 0; j < m2->nb_columns; j++)
        {
            float sum = 0;
            for (k = 0; k < m1->nb_columns; k++)
            {
                sum += getValue(m1, i, k) * getValue(m2, k, j);
            }
            setValue(&result, i, j, sum);
        }
    }
    return result;
}

int check(struct Matrix *m, struct Matrix *result)
{
    struct Matrix B = multiplyMatrix(m, result);

    for (int i = 0; i < m->nb_lines; i++)
    {
        for (int j = 0; j < m->nb_columns; j++)
        {
            float diff = getValue(&B, i, j) - getValue(result, i, j);
            if (diff > 1e-6 || diff < -1e-6)
            {
                return 0;
            }
        }
    }
    return 1;
}
const int MASTER_RANK = 0;
int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int name_len;

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
        int n;
        fscanf(file, "%d", &n);
        // if (n != m)
        // {
        //     printf("🐛 Matrix is not square!\n");
        //     return 1;
        // }

        // check if matrix is not too big
        if (n > 100)
        {
            printf("🐛 Matrix is too big!\n");
            return 2;
        }

        int quantity = n * n;
        // check if there is equal quantity of numbers in file
        int i = 0;
        int number;

        // go to next line
        fscanf(file, "%*[^\n]\n");

        while (fscanf(file, "%d", &number) != EOF)
        {
            i++;
        }
        if (i != quantity)
        {
            printf("🐛 Wrong quantity of numbers!\n");
            return 2;
        }

        // set readnig from second line
        fseek(file, 3, SEEK_SET);

        allocateMatrix(&matrix, n, n);
        // take matrix from file
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
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

    printf("Macierz:\n");
    printMatrix(&matrix);

    struct Matrix inversMatrix;
    allocateMatrix(&inversMatrix, matrix.nb_columns, matrix.nb_columns);
    makeIdentity(&inversMatrix);

    // print 'inversMatrix':
    printf("Macierz jednostkowa:\n");
    printMatrix(&inversMatrix);

    // pomiar czasu
    double timeBegin;
    if (world_rank == MASTER_RANK)
    {
        timeBegin = MPI_Wtime();
    }

    int isSingular = 0;
    isSingular = gaussJordanElimination(&matrix, &inversMatrix);

    if (world_rank == MASTER_RANK)
    {
        if (isSingular)
        {
            printf("😱mamcierz jest osobliwa😱");
            return 3;
        }
        else
        {
            // pomiar czasu
            double timeSpend = MPI_Wtime() - timeBegin;
            printf("czas obiczen: %.6f [s]", timeSpend);

            // printf("Macierz:\n");
            // printMatrix(&matrix);

            // print 'inversMatrix':
            printf("Macierz odwrotna:\n");
            printMatrix(&inversMatrix);

            // checking for correctness of inversMatrix:
            if (check(&matrix, &inversMatrix))
            {
                printf("👌 Macierz odwrotna jest poprawna!\n");
            }
            else
            {
                printf("🐛 Macierz odwrotna jest niepoprawna!\n");
            }
        }
    }
    // Finalize the MPI environment.
    MPI_Finalize();
}