// import libraries
#include <stdio.h>


void gaussJordanInversion(float **matrix , float **inversMatrix, int quantityOfRows)
{
    int i, j, k, l;
    float temp;
    //macierz jednostkowa
    for (i = 0; i < quantityOfRows; i++)
    {
        for (j = 0; j < quantityOfRows; j++)
        {
            if (i == j)
            {
                inversMatrix[i][j] = 1;
            }
            else
            {
                inversMatrix[i][j] = 0;
            }
        }
    }

    for (i = 0; i < quantityOfRows; i++)
    {
        for (j = 0; j < quantityOfRows; j++)
        {
            if (matrix[i][j] != 0)
            {
                break;
            }
        }
        if (j == quantityOfRows)
        {
            printf("Macierz jest nieodwracalna\n");
            return;
        }
        if (matrix[i][j] != 1)
        {
            temp = matrix[i][j];
            for (k = 0; k < quantityOfRows; k++)
            {
                matrix[i][k] /= temp;
                inversMatrix[i][k] /= temp;
            }
        }
        for (k = 0; k < quantityOfRows; k++)
        {
            if (k != i)
            {
                temp = matrix[k][j];
                for (l = 0; l < quantityOfRows; l++)
                {
                    matrix[k][l] -= matrix[i][l] * temp;
                    inversMatrix[k][l] -= inversMatrix[i][l] * temp;
                }
            }
        }
    }

}

int main(int argC, char **args)
{
    // check if there is only one argument
    if (argC != 2)
    {
        printf("Wrong number of arguments!\n");
        return 1;
    }

    // open file
    FILE *file = fopen(args[1], "r");
    if (file == NULL)
    {
        printf("Cannot open file!\n");
        return 1;
    }

    // check corectness of file
    
    // take two numbers form first line and check if they are correclt
    int n, m;
    fscanf(file, "%d %d", &n, &m);
    if (n != m)
    {
        printf("Matrix is not square!\n");
        return 1;
    }

    // check if matrix is not too big
    if (n > 100)
    {
        printf("Matrix is too big!\n");
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
        printf("Wrong quantity of numbers!\n");
        return 1;
    }
    // set readnig from second line
    fseek(file, 0, SEEK_SET);

    float **matrix;
    // alocate memory for matrix
    matrix = (float **)malloc(n * sizeof(float *));

    // take matrix from file
    // float matrix[n][m];
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            fscanf(file, "%f", &matrix[i][j]);
        }
    }   

    // close file
    fclose(file);


    // print: 'matrix':
    printf("Macierz:\n");

    // print matrix
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }

    // create matrix for result
    float result[n][n];

    gaussJordanInversion(matrix, result, n);

    // print: 'inversMatrix':
    printf("Macierz odwrotna:\n");

    // print result
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%f ", result[i][j]);
        }
        printf("\n");
    }
}