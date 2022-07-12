// Autorzy: Patryk Jaseniuk; Franciszek Hajduk @ Konto: PR1g1
// Cwiczenie: 2, Data opracowania: 2022-07-05, Wersja: 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argC, char **arg)
{
    int iloscLosowan = 1000;
    if (argC > 1)
    {
        iloscLosowan = atoi(arg[1]);
    }

    clock_t begin = clock();

    float xMin = 0;
    float xMax = 2;
    float yMin = 0;
    float yMax = 1.1;

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
    float pole = (xMax - xMin) * (yMax - yMin);
    float calka = alfa * pole;

    // calculate time difference
    clock_t TEnd = clock();
    double timeSpend = (double)(TEnd - begin) / CLOCKS_PER_SEC;

    printf("\n");
    printf("---------------------------------------------------.\n");
    printf("Obliczanie oszacowania wartosci calki oznaczonej\n");
    printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
    printf("---------------------------------------------------.\n");
    printf("Autorzy: Jaseniuk Patryk, Hajduk Franciszek\n");
    printf("Grupa: L1/G2 \n");
    printf("---------------------------------------------------.\n");
    printf("Liczba losowan: %d\n", iloscLosowan);        // liczba całkowita
    printf("Oszacowanie wartosci calki: %.4f\n", calka); // liczba zmiennoprzecinkowa, 4 cyfry po przecinku
    printf("Czas obliczen [s]: %.6f \n", timeSpend);     // liczba zmiennoprzecinkowa, 6 cyfr po przecinku
    printf("---------------------------------------------------.\n");
    printf("\n");

    FILE *fp = fopen("2_JaseniukHajduk.lst", "a"); // zapis wyników do pliku .lst
    if (fp != NULL)
    {
        fprintf(fp, "Liczba losowan: %d, Oszacowanie wartosci calki: %.4f, Czas obliczen [s]: %.6f\n", iloscLosowan, calka, timeSpend);
        fclose(fp);
    }
    return 0;
}
