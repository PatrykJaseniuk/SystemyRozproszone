

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
    // maksymalnie równolegle może działać tyle procesów ile jest wierszy ponizej aktualnego elementu przekatnej

    // poszczegolne iteracje petli mogą odbywać się równolegle ponieważ każdy i-ty obieg petli odczytuje i modyfikuje inne zmienne(inne wiersze macierzy)
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

// oblicza macierz odwrotna do podanej
// `matrix` - macierz do obrocenia, po obliczeniach bedzie jednostkową
// `inverseMatrix` - wysłana macierz musi być jednstkowa po obliczeniach bedzie macierzą odwrotna do `matrix`
// OPIS ALGORYTMU :
// -dozwolone operacje elementarne:
//      1)możenie dowolnego wiersza przez dowoly skalar,
//      2)dodawanie do dowolnego wiersza dowolnej wilokrotności innego
// -mamy dwie macierze:
//      1)pierwsza do obrócenia,
//      2)druga jednostkowa
// -każda operacja jest bliźniacza tzn musi być wykonana na jednej i drugiej macierzy
// -za pomacą operacji elementarnych tak modyfikujemu pierwszą macież aby doprowadzić ją do macierzy jednostkowej.
//  Wtedy w tejemniczy sposób druga macierz, która byłą na początku jednostkową po tych wszystkich operacjach stanie się odwrotną do tej pierwszej.
// STRATEGIA UZYSKIWANIA ZA POMOCĄ OPERACJI ELEMENTARNYCH MACIERZY JEDNOSTKOWEJ:
// 1)zerowanie macierzy poniżej przekątnej:
//      {dla każdej kolumny `k` :
//          zeruj kolumne `k` poniżej przekątnej(k):<-----te operację mogą wykonywać się jednocześnie-------
//           {  weź element przekątnej na kolumnie `k` i zapisz do `factor`
//              podziel wiersz `k` prze `faktor` tak że teraz element k przekatnej jest równy 1
//              odejmuj od wszystkich poniższych wierszy wielokrotność wiersza `k` i elementu [i,k](`i` to aktualny wiersz, `k` to aktualna kolumna) tak że w całej kolumnie otrzymujesz 0 poniżej przekątnej
//            }
//      }
// 2)zerowanie macierzy powyżej przekątnej: podobnie do punktu 1) tylko w drugą stronę
// ZRÓWNOLEGLANIE:
// zaznaczyłem strzałka gdzie mogę zrównoleglić operacje
// maksymalnie można jednocześnie wywołać tyle procesów ile jest wierszy do wyzerowania poniżej aktualnego elementu przekątnej
//  jeżeli procesów jest mniej to tzreba jakoż podzielić wiersze pomiędzy procesy
//  można by jeszcze samą operację mnożenia wiersza przez liczbę i dodawania do innego wiersza zrównoleglić, ale nie będe tego robić
//
char gaussJordanElimination(struct Matrix *matrix, struct Matrix *inversMatrix)
{
    zeroLowerTriangle(matrix, inversMatrix);
    zeroUpperTriangle(matrix, inversMatrix);

    return isSingular;
}