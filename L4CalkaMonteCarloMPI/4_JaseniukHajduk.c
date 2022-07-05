
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argC, char **arg)
{
    int iloscLosowan = 1000;
    if (argC > 1)
    {
        iloscLosowan = atoi(arg[1]);
    }

    MPI_Init(&argc, &argv); // inicjalizacja mpi

    double begin = MPI_Wtime(); // pomiar czasu
    // clock_t begin = clock();

    // get cuttent tick count
    // clock_t begin = clock();

    ///   float calka = obliczCalke(iloscLosowan);
    float xMin = 0;
    float xMax = 2;
    float yMin = 0;
    float yMax = 1.1;

    // float alfa = obliczAlfa(xMin, xMax, yMin, yMax, iloscLosowan);
    float seed = begin;
    int licznikTrafien = 0; // niezainicjalizowana zmienna przyjmuje wartosc losowa
    srand48(seed);
    for (int i = 0; i < iloscLosowan; i++)
    {
        float randX = drand48() * xMax;
        float randY = drand48() * yMax;
        float wynik = sqrt(randX * (2 - randX));
        if (randY < wynik)
        {
            licznikTrafien++;
        }
    }
    float alfa = (float)licznikTrafien / (float)iloscLosowan;
    // float alfa = obliczAlfa(xMin, xMax, yMin, yMax, iloscLosowan);

    // float pole = obliczPole(xMin, xMax, yMin, yMax);
    float pole = (xMax - xMin) * (yMax - yMin);
    // float pole = obliczPole(xMin, xMax, yMin, yMax);

    float calka = alfa * pole;
    ///   float calka = obliczCalke(iloscLosowan);

    // calculate time difference
    // clock_t TEnd = clock();
    double timeSpend = MPI_Wtime() - begin;
    // double timeSpend = (double)(TEnd - begin) / CLOCKS_PER_SEC;

    MPI_Finalize(); // zakonczenie mpi

    // clock_t end = clock();
    // int time_spent = (int)((end - begin) / CLOCKS_PER_SEC);

    printf("\n");
    printf("---------------------------------------------------.\n");
    printf("Obliczanie oszacowania wartosci calki oznaczonej\n");
    printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
    printf("---------------------------------------------------.\n");
    printf("Autorzy: Patryk Jaseniuk Franciszek Hajduk\n");
    printf("Grupa: L1/G1 \n");
    printf("---------------------------------------------------.\n");
    printf("Liczba losowan: %d\n", iloscLosowan);        // liczba całkowita
    printf("Oszacowanie wartosci calki: %.4f\n", calka); // liczba zmiennoprzecinkowa, 4 cyfry po przecinku
    printf("Czas obliczen [s]: %.6f \n", timeSpend);     // liczba zmiennoprzecinkowa, 6 cyfr po przecinku
    printf("---------------------------------------------------.\n");
    printf("\n");

    FILE *fp = fopen("4_z1JaseniukHajduk.4.lst", "a"); // zapis wyników do pliku .lst
    if (fp != NULL)
    {
        fprintf(fp, "Liczba losowan: %d, Oszacowanie wartosci calki: %.4f, Czas obliczen [s]: %.6f\n", iloscLosowan, calka, timeSpend);
        fclose(fp);
    }

    return 0;
}
