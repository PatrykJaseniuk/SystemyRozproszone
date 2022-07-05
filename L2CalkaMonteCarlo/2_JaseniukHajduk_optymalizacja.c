
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


// char *odpowiedz(int iloscLosowan, float wynikCalki, float czasObliczen)
// {
//     char text[20];
//     char *odpowiedz = (char *)malloc(10000 * sizeof(char));
//     strcat(odpowiedz, "\n");
//     strcat(odpowiedz, "Obliczanie oszacowania wartości calki oznaczonej\n");
//     strcat(odpowiedz, "z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
//     strcat(odpowiedz, "---------------------------------------------------.\n");
//     strcat(odpowiedz, "Autor(-rzy): Patryk Jaseniuk, Franciszek Hajduk");
//     strcat(odpowiedz, "Grupa: L1/G3\n");
//     strcat(odpowiedz, "---------------------------------------------------\n");
//     strcat(odpowiedz, "Liczba losowan: ");
//     sprintf(text, "%d", iloscLosowan);
//     strcat(odpowiedz, text);

//     strcat(odpowiedz, "\nOszacowanie wartości calki: ");
//     gcvt(wynikCalki, 6, text);
//     strcat(odpowiedz, text);
//     strcat(odpowiedz, "\nCzas obliczeń [s]: ");
//     gcvt(czasObliczen, 6, text);
//     // sprintf(text, "%d", czasObliczen);
//     strcat(odpowiedz, text);
//     strcat(odpowiedz, "\n");
//     return odpowiedz;
// }

// void zapiszDoPliku(char *odpowiedx)
// {
//     FILE *f = fopen("calka.lst", "a");
//     if (f == NULL)
//     {
//         printf("Error opening file!\n");
//         exit(1);
//     }

//     /* print some text */
//     // const char *text = "Write this to the file";
//     fprintf(f, "%s\n\n", odpowiedx);
// }

int main(int argC, char **arg)
{
    int iloscLosowan = 1000;
    if (argC > 1)
    {
        iloscLosowan = atoi(arg[1]);
    }

    // clock_t begin = clock();

    // get cuttent tick count
    clock_t begin = clock();

    ///   float calka = obliczCalke(iloscLosowan);
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

    // clock_t end = clock();
    // int time_spent = (int)((end - begin) / CLOCKS_PER_SEC);

    printf("\n");
    printf("---------------------------------------------------.\n");
    printf("Obliczanie oszacowania wartosci calki oznaczonej\n");
    printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
    printf("---------------------------------------------------.\n");
    printf("Autorzy: Jaseniuk Patryk, hajduk Franciszek\n");
    printf("Grupa: L1/G2 \n");
    printf("---------------------------------------------------.\n");
    printf("Liczba losowan: %d\n", iloscLosowan);                      //liczba całkowita
    printf("Oszacowanie wartosci calki: %.4f\n", calka);                    //liczba zmiennoprzecinkowa, 4 cyfry po przecinku
    printf("Czas obliczen [s]: %.6f \n", timeSpend);                        //liczba zmiennoprzecinkowa, 6 cyfr po przecinku
    printf("---------------------------------------------------.\n");
    printf("\n");


    FILE* fp = fopen("2_JaseniukHajduk.lst", "a");			    //zapis wyników do pliku .lst
    if (fp != NULL)
    {
        fprintf(fp, "Liczba losowan: %d, Oszacowanie wartosci calki: %.4f, Czas obliczen [s]: %.6f\n", iloscLosowan, calka, timeSpend);
        fclose(fp);
    }

    return 0;
}
