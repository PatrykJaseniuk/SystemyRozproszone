// Autorzy: Patryk Jaseniuk; Franciszek Hajduk @ Konto: PR1g1
// Cwiczenie: 2, Data opracowania: 2022-07-05, Wersja: 2

// import libraries
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
// #include "Matrix.h"

// start "Matrix.h"
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

void zeroKolumnBelowDiagonal(struct Matrix *m, struct Matrix *result, int column)
{
    // make diagonal 1
    float diagonal = getValue(m, column, column);
    if (diagonal <= 1e-6 && diagonal >= -1e-6)
    {
        isSingular = 1;
        return;
    }
    multiplyRowByScalar(m, column, 1.0 / diagonal);
    multiplyRowByScalar(result, column, 1.0 / diagonal);

    // tutaj zrownolegle

    for (int row = column + 1; row < m->nb_lines; row++)
    {
        float factor = getValue(m, row, column);
        multiplyRowByScalarAndAddToRow(m, row, -factor, column);
        multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    }
}

void zeroLowerTriangle(struct Matrix *m, struct Matrix *result)
{
    for (int column = 0; column < m->nb_columns; column++)
    {
        zeroKolumnBelowDiagonal(m, result, column);
        if (isSingular)
        {
            return;
        }
    }
}
void zeroKolumnAboveDiagonal(struct Matrix *m, struct Matrix *result, int column)
{
    for (int row = column - 1; row >= 0; row--)
    {
        float factor = getValue(m, row, column);
        multiplyRowByScalarAndAddToRow(m, row, -factor, column);
        multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    }
}
void zeroUpperTriangle(struct Matrix *m, struct Matrix *result)
{
    for (int coumn = m->nb_columns - 1; coumn >= 0; coumn--)
    {
        zeroKolumnAboveDiagonal(m, result, coumn);
    }
}

char gaussJordanElimination(struct Matrix *matrix, struct Matrix *inversMatrix)
{
    zeroLowerTriangle(matrix, inversMatrix);
    zeroUpperTriangle(matrix, inversMatrix);

    return isSingular;
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

int checking(struct Matrix *m, struct Matrix *result)
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
//  end "Matrix.h"

int main(int argC, char **args)
{

    struct Matrix matrix;
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

    // print size of matrix
    printf("sieze of matrix: %ld\n", sizeof(matrix));

    printf("Macierz:\n");
    printMatrix(&matrix);

    struct Matrix inversMatrix;
    allocateMatrix(&inversMatrix, matrix.nb_columns, matrix.nb_columns);
    makeIdentity(&inversMatrix);

    // print 'inversMatrix':
    printf("Macierz jednostkowa:\n");
    printMatrix(&inversMatrix);

    int isSingular = 0;

    clock_t begin = clock();
    isSingular = gaussJordanElimination(&matrix, &inversMatrix);
    clock_t end = clock();
    float time_spent = ((float)(end - begin) / CLOCKS_PER_SEC);
    // print: 'matrix':

    if (isSingular)
    {
        printf("😱mamcierz jest osobliwa😱");
        return 3;
    }
    else
    {
        // printf("Macierz:\n");
        // printMatrix(&matrix);

        printf("czas obliczeń %.6f [s] \n", time_spent);

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
}