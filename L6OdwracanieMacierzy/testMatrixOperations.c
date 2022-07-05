#include "Matrix.h"

int main()
{
    struct Matrix matrix;
    allocateMatrix(&matrix, 3, 3);
    fillMatrixWithAscendingValues(&matrix);
    setValue(&matrix, 2, 2, 11);
    printMatrix(&matrix);

    // float factor = 777777777777777777777.0;
    // int row = 0;
    // int row2 = 1;

    // printf("multiplyRowByScalar(&matrix, %d, %f);:\n", row, factor);
    // multiplyRowByScalar(&matrix, row, factor);
    // printMatrix(&matrix);

    // factor = 1 / factor;
    // printf("multiplyRowByScalar(&matrix, %d, %f);:\n", row, factor);
    // multiplyRowByScalar(&matrix, row, factor);
    // printMatrix(&matrix);

    // factor = 4;
    // printf("multiplyRowByScalarAndAddToRow(&matrix, %d, %f, %d);:\n", row, factor, row2);
    // multiplyRowByScalarAndAddToRow(&matrix, row, factor, row2);
    // printMatrix(&matrix);

    struct Matrix result;
    allocateMatrix(&result, 3, 3);
    makeIdentity(&result);

    printf("gausJordanElimination(&matrix, &result);:\n");
    gausJordanElimination(&matrix, &result);
    printMatrix(&result);
    printMatrix(&matrix);

    // zeroKolumnBelowDiagonal(&matrix, &result, 0);
    // int column = 0;
    // printf("zeroKolumnBelowDiagonal(&matrix, &result, %d);:\n", column);
    // zeroKolumnBelowDiagonal(&matrix, &result, column);
    // printMatrix(&result);
    // printMatrix(&matrix);
    return 0;
}