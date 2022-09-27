#include "Matrix.h"

int main()
{
    int sizeOfMatrix = 3;
    float *matrix;
    // allocate memory for matrix
    matrix = malloc(sizeOfMatrix * sizeOfMatrix * sizeof(float));

    // fill matrix with asccending numbers
    for (int i = 0; i < sizeOfMatrix * sizeOfMatrix; i++)
    {
        matrix[i] = i;
    }
    // print: 'matrix':
    printf("Macierz:\n");

    // print matrix
    for (int i = 0; i < sizeOfMatrix; i++)
    {
        for (int j = 0; j < sizeOfMatrix; j++)
        {
            printf("%f ", matrix[i * sizeOfMatrix + j]);
        }
        printf("\n");
    }

    // print: 'matrix':
    printf("Macierz 2:\n");

    // print matrix
    for (int i = 0; i < sizeOfMatrix; i++)
    {
        for (int j = 0; j < sizeOfMatrix; j++)
        {
            printf("%f ", matrix[i]);
        }
        printf("\n");
    }

    // matrix as two dimensional array
    float **matrix2 = malloc(sizeOfMatrix * sizeof(float *));
    for (int i = 0; i < sizeOfMatrix; i++)
    {
        matrix2[i] = malloc(sizeOfMatrix * sizeof(float));
    }

    // fill matrix2 with asccending numbers
    for (int i = 0; i < sizeOfMatrix; i++)
    {
        for (int j = 0; j < sizeOfMatrix; j++)
        {
            matrix2[i][j] = i * sizeOfMatrix + j;
        }
    }

    // print: 'matrix2':
    printf("Macierz 2:\n");

    // print matrix2
    for (int i = 0; i < sizeOfMatrix; i++)
    {
        for (int j = 0; j < sizeOfMatrix; j++)
        {
            printf("%f ", matrix2[i][j]);
        }
        printf("\n");
    }

    // matrix 3 in structural form
    struct Matrix matrix3;

    //   ask user for matrix size
    printf("Podaj ilosc wierszy: ");
    scanf("%d", &matrix3.nb_lines);
    printf("Podaj ilosc kolumn: ");
    scanf("%d", &matrix3.nb_columns);

    // allocate memory for matrix3
    allocateMatrix(&matrix3, matrix3.nb_lines, matrix3.nb_columns);

    // fill matrix3 with asccending numbers
    fillMatrixWithAscendingValues(&matrix3);

    printMatrix(&matrix3);

    return 0;
}