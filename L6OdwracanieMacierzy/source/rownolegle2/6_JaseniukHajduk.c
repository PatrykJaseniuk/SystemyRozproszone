// Autorzy: Patryk Jaseniuk; Franciszek Hajduk @ Konto: PR1g1
// Cwiczenie: 2, Data opracowania: 2022-07-05, Wersja: 2

// komentarze są troche chaotyczne bo część jest po angielsku a część po polsku

// import libraries
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <mpi.h>

const int MASTER_RANK = 0;
int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];
int name_len;

// uzywam zmiennej globalnej zamiast zwracać warotść przez funkcje dlatego że potrzebuję wyjść z kilku poziomów wywołanych funkcji.
char isSingular = 0;

void handelError(int result)
{
    // get error message
    char error_message[MPI_MAX_ERROR_STRING];
    int error_string_length;
    MPI_Error_string(result, error_message, &error_string_length);
    printf("Error in processor:%s, rank: %d\n message: %s\n", processor_name, world_rank, error_message);
    MPI_Abort(MPI_COMM_WORLD, 1);
}

// struktura w której zapisana będzie macierz (psełdo obiektowość)
struct Matrix
{
    int nb_lines;
    int nb_columns;
    float *data;
};

// podstawowe operacje na macierzy
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

// Funkcje służące do odwrocenia macierzy metodą Gausa-Jordana
void zeroKolumnBelowDiagonal(struct Matrix *m, struct Matrix *result, int column)
{
    // printf("zerowanie kolumny%d", column);
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

    // podziel wiersze do modyfikacji pomiedzy dostępnych robotników
    int pozostaleWiersze = m->nb_lines - column - 1;
    int wierszeNaRobotnika = pozostaleWiersze / world_size;
    int resztaWierszy = pozostaleWiersze % (world_size); // master weźmie jeden przydział i resztę

    // wyślij każdemu aktualną macierz i nr wierszy do modyfikacji i nr zerowanej kolumny
    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++)
    {
        // wyslij info żeby pracownik dzialal
        int dzialaj = 1;
        (MPI_Send(&dzialaj, 1, MPI_INT, nrPracownika, 10, MPI_COMM_WORLD));

        (MPI_Send((m->data), m->nb_lines * m->nb_lines, MPI_FLOAT, nrPracownika, 1, MPI_COMM_WORLD));
        (MPI_Send((result->data), result->nb_lines * result->nb_lines, MPI_FLOAT, nrPracownika, 1, MPI_COMM_WORLD));

        int pierwszyIOstatniWiersz[2];
        pierwszyIOstatniWiersz[0] = column + 1 + (nrPracownika - 1) * wierszeNaRobotnika;
        pierwszyIOstatniWiersz[1] = pierwszyIOstatniWiersz[0] + wierszeNaRobotnika;

        (MPI_Send(pierwszyIOstatniWiersz, 2, MPI_INT, nrPracownika, 2, MPI_COMM_WORLD));

        (MPI_Send(&column, 1, MPI_INT, nrPracownika, 3, MPI_COMM_WORLD));
    }

    // odbierz zmodyfikowane wiersze od robotników i scal je w nową macierz
    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++)
    {
        int pierwszyIOstatniWiersz[2];
        pierwszyIOstatniWiersz[0] = column + 1 + (nrPracownika - 1) * wierszeNaRobotnika;
        pierwszyIOstatniWiersz[1] = pierwszyIOstatniWiersz[0] + wierszeNaRobotnika;
        int iloscDanychDoOdebrania = wierszeNaRobotnika * m->nb_columns;
        float *wskNaPoczatekDanych1 = ((m)->data) + (pierwszyIOstatniWiersz[0] * (m)->nb_columns); // dodawanie do wskaźnika przesuwa wskaźnik
        float *wskNaPoczatekDanych2 = ((result)->data) + (pierwszyIOstatniWiersz[0] * (result)->nb_columns);

        MPI_Recv(wskNaPoczatekDanych1, iloscDanychDoOdebrania, MPI_FLOAT, nrPracownika, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(wskNaPoczatekDanych2, iloscDanychDoOdebrania, MPI_FLOAT, nrPracownika, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    // printf("odebrana macierz:\n");
    // printMatrix(m);

    // maser robi swoj przydzial
    for (int row = m->nb_lines - wierszeNaRobotnika - resztaWierszy; row < m->nb_lines; row++)
    {
        float factor = getValue(m, row, column);
        multiplyRowByScalarAndAddToRow(m, row, -factor, column);
        multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    }

    // printf("macierz po masterze:\n");
    // printMatrix(m);

    // // zatrzymanie
    // MPI_Recv(m, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // wersjasekwencyjna
    // for (int row = column + 1; row < m->nb_lines; row++)
    // {
    //     float factor = getValue(m, row, column);
    //     multiplyRowByScalarAndAddToRow(m, row, -factor, column);
    //     multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    // }
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

    // podziel wiersze do modyfikacji pomiedzy dostępnych robotników
    int pozostaleWiersze = column;
    int wierszeNaRobotnika = pozostaleWiersze / world_size;
    int resztaWierszy = pozostaleWiersze % (world_size); // master weźmie jeden przydział i resztę

    // wyślij każdemu aktualne macierze i nr wierszy do modyfikacji i nr zerowanej kolumny
    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++) // lepiej by był użyć MPI_Bcast ale coś mi nie działało
    {
        // wyslij info żeby pracownik dzialal
        int dzialaj = 1;
        (MPI_Send(&dzialaj, 1, MPI_INT, nrPracownika, 10, MPI_COMM_WORLD));

        (MPI_Send((m->data), m->nb_lines * m->nb_lines, MPI_FLOAT, nrPracownika, 1, MPI_COMM_WORLD));
        (MPI_Send((result->data), result->nb_lines * result->nb_lines, MPI_FLOAT, nrPracownika, 1, MPI_COMM_WORLD));

        int pierwszyIOstatniWiersz[2];
        pierwszyIOstatniWiersz[0] = column - 1 - (nrPracownika - 1) * wierszeNaRobotnika;
        pierwszyIOstatniWiersz[1] = pierwszyIOstatniWiersz[0] - wierszeNaRobotnika;

        (MPI_Send(pierwszyIOstatniWiersz, 2, MPI_INT, nrPracownika, 2, MPI_COMM_WORLD));

        (MPI_Send(&column, 1, MPI_INT, nrPracownika, 3, MPI_COMM_WORLD));
    }

    // maser robi swoj przydzial
    for (int row = column - 1; row >= 0; row--)
    {
        float factor = getValue(m, row, column);
        printf("factor: %f", factor);
        multiplyRowByScalarAndAddToRow(m, row, -factor, column);
        multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    }

    // odbierz zmodyfikowane wiersze od robotników i scal je w nową macierz
    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++)
    {
        int pierwszyIOstatniWiersz[2];
        pierwszyIOstatniWiersz[0] = column - 1 - (nrPracownika - 1) * wierszeNaRobotnika;
        pierwszyIOstatniWiersz[1] = pierwszyIOstatniWiersz[0] - wierszeNaRobotnika;
        int iloscDanychDoOdebrania = wierszeNaRobotnika * m->nb_columns;
        float *wskNaPoczatekDanych1 = ((m)->data) + ((pierwszyIOstatniWiersz[1] + 1) * (m)->nb_columns); // dodawanie do wskaźnika przesuwa wskaźnik
        float *wskNaPoczatekDanych2 = (result->data) + ((pierwszyIOstatniWiersz[1] + 1) * (result)->nb_columns);

        MPI_Recv(wskNaPoczatekDanych1, iloscDanychDoOdebrania, MPI_FLOAT, nrPracownika, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(wskNaPoczatekDanych2, iloscDanychDoOdebrania, MPI_FLOAT, nrPracownika, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    // printf("odebrana macierz:\n");
    // printMatrix(m);

    // for (int row = column - 1; row >= 0; row--)
    // {
    //     float factor = getValue(m, row, column);
    //     multiplyRowByScalarAndAddToRow(m, row, -factor, column);
    //     multiplyRowByScalarAndAddToRow(result, row, -factor, column);
    // }
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
void gaussJordanElimination(struct Matrix *matrix, struct Matrix *inversMatrix)
{
    // wyslij info do robotników o rozmiarze macierzy
    // MPI_Bcast(&matrix->nb_lines, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++)
    {
        MPI_Send(&matrix->nb_lines, 1, MPI_INT, nrPracownika, 0, MPI_COMM_WORLD);
    }

    zeroLowerTriangle(matrix, inversMatrix);
    zeroUpperTriangle(matrix, inversMatrix);
}

// operacje wykonywane przez proces master
int master(int argC, char **args)
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

    clock_t begin = clock();
    gaussJordanElimination(&matrix, &inversMatrix);
    clock_t end = clock();
    float time_spent = ((float)(end - begin) / CLOCKS_PER_SEC);
    // print: 'matrix':

    if (isSingular)
    {
        printf("😱 macierz jest osobliwa 😱\n");
        return 3;
    }
    else
    {
        printf("Macierz:\n");
        printMatrix(&matrix);

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

    for (int nrPracownika = 1; nrPracownika < world_size; nrPracownika++) // lepiej by był użyć MPI_Bcast ale coś mi nie działało
    {
        // wyslij info żeby pracownik przestal dzialac
        int dzialaj = 0;
        (MPI_Send(&dzialaj, 1, MPI_INT, nrPracownika, 10, MPI_COMM_WORLD));
    }
}

void worker()
{
    // printf("worker rank: %d is working\n", world_rank);

    // odbierz info o rozmiarze macierzy
    int rozmiar;
    MPI_Recv(&rozmiar, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // printf("worker[%d] rozmiar: %d\n", world_rank, rozmiar);

    // incicjalizu macierze
    struct Matrix m1, m2; // dlaczego jest problem kiedy zdefiniuje wskaxniki?
    allocateMatrix(&m1, rozmiar, rozmiar);
    allocateMatrix(&m2, rozmiar, rozmiar);
    // printf("worker[%d] inicialized matrix: \n", world_rank);
    // printMatrix(&m1);

    int czyDzialac = 1;

    while (czyDzialac)
    {
        MPI_Recv(&czyDzialac, 1, MPI_INT, MASTER_RANK, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (!czyDzialac)
        {
            break;
        }
        // odbierz macierze i nr wierszy do modyfikacji i nr zerowanej kolumny
        MPI_Recv(((&m1)->data), (&m1)->nb_lines * (&m1)->nb_lines, MPI_FLOAT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(((&m2)->data), (&m2)->nb_lines * (&m2)->nb_lines, MPI_FLOAT, MASTER_RANK, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("worker[%d] get matrix: \n", world_rank);
        // printMatrix(&m1);

        int pierwszyIOstatniWiersz[2];
        MPI_Recv(pierwszyIOstatniWiersz, 2, MPI_INT, MASTER_RANK, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("worker[%d] get lines: %d:%d \n", world_rank, pierwszyIOstatniWiersz[0], pierwszyIOstatniWiersz[1]);

        int kolumnaDoZerowania;
        MPI_Recv(&kolumnaDoZerowania, 1, MPI_INT, MASTER_RANK, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int czyZerowanieDolnegoTrojkata = pierwszyIOstatniWiersz[0] <= pierwszyIOstatniWiersz[1];
        if (czyZerowanieDolnegoTrojkata)
        {
            // modyfikuj wiersze
            for (int row = pierwszyIOstatniWiersz[0]; row < pierwszyIOstatniWiersz[1]; row++)
            {
                float factor = getValue(&m1, row, kolumnaDoZerowania);
                multiplyRowByScalarAndAddToRow(&m1, row, -factor, kolumnaDoZerowania);
                multiplyRowByScalarAndAddToRow(&m2, row, -factor, kolumnaDoZerowania);
            }

            // prześlij zmodyfikowane wiersze
            int iloscDanychDoPrzeslania = (pierwszyIOstatniWiersz[1] - pierwszyIOstatniWiersz[0]) * (&m1)->nb_columns;
            float *wskNaPoczatekDanych1 = ((&m1)->data) + (pierwszyIOstatniWiersz[0] * (&m1)->nb_columns); // dodawanie do wskaźnika przesuwa wskaźnik
            float *wskNaPoczatekDanych2 = ((&m2)->data) + (pierwszyIOstatniWiersz[0] * (&m2)->nb_columns); //
            (MPI_Send(wskNaPoczatekDanych1, iloscDanychDoPrzeslania, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD));
            (MPI_Send(wskNaPoczatekDanych2, iloscDanychDoPrzeslania, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD));
        }
        else // zerowanie gornego trojkata
        {
            // modyfikuj wiersze
            printf("worker[%d] get lines: %d:%d \n", world_rank, pierwszyIOstatniWiersz[0], pierwszyIOstatniWiersz[1]);

            for (int row = pierwszyIOstatniWiersz[0]; row > pierwszyIOstatniWiersz[1]; row--)
            {
                float factor = getValue(&m1, row, kolumnaDoZerowania);
                // float factor = 10;
                multiplyRowByScalarAndAddToRow(&m1, row, -factor, kolumnaDoZerowania);
                multiplyRowByScalarAndAddToRow(&m2, row, -factor, kolumnaDoZerowania);
            }

            // multiplyRowByScalar(&m1, 0, 10);
            // multiplyRowByScalar(&m1, 1, 10);
            // multiplyRowByScalar(&m1, 2, 10);
            // multiplyRowByScalar(&m1, 3, 10);
            // multiplyRowByScalar(&m1, 4, 10);
            // multiplyRowByScalar(&m2, 0, 10);
            // multiplyRowByScalar(&m2, 1, 10);
            // multiplyRowByScalar(&m2, 2, 10);
            // // multiplyRowByScalar(&m2, 3, 10);
            // multiplyRowByScalar(&m2, 4, 10);
            // prześlij zmodyfikowane wiersze
            int iloscDanychDoPrzeslania = (pierwszyIOstatniWiersz[0] - pierwszyIOstatniWiersz[1]) * (&m1)->nb_columns;
            float *wskNaPoczatekDanych1 = ((&m1)->data) + ((pierwszyIOstatniWiersz[1] + 1) * (&m1)->nb_columns); // dodawanie do wskaźnika przesuwa wskaźnik
            float *wskNaPoczatekDanych2 = ((&m2)->data) + ((pierwszyIOstatniWiersz[1] + 1) * (&m2)->nb_columns); //
            (MPI_Send(wskNaPoczatekDanych1, iloscDanychDoPrzeslania, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD));
            (MPI_Send(wskNaPoczatekDanych2, iloscDanychDoPrzeslania, MPI_FLOAT, MASTER_RANK, 0, MPI_COMM_WORLD));
        }
    }
}

int main(int argC, char **args)
{
    int result;
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

    if (world_rank == MASTER_RANK)
    {
        result = master(argC, args);
    }
    else
    {
        worker();
    }

    // Finalize the MPI environment.
    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return result;
}