// Autorzy: Oktawian Kusy; Dominik Mikula; @ Konto: PR1g2
// Cwiczenie: 2, Data opracowania: 2022-06-25, Wersja: 4
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define srand48(x) srand((int)(x))
#define drand48() ((double)rand()/RAND_MAX)
#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
    double x, y, p, t_start, t_end;
    int licznik_trafien = 0;
    if (argc != 2)
    {                                                           //sprawdzenie ilosci parametrow
        printf("Niewlasciwa liczba parametrow!\n");
        return 1;
    }
    int ilosc_losowan = atoi(argv[1]);                      //ustalenie liczby losowań na podstawie podanego parametru
    if (ilosc_losowan < 1)
    {
        printf("Liczba losowan nie jest dodatnia!\n");
        return 2;
    }


    MPI_Init (&argc, &argv);							//inicjalizacja mpi

    t_start = MPI_Wtime();								        //pomiar czasu przed rozpoczęciem obliczeń
    srand48(t_start);									        //ustalenie ziarna algorytmu losowania na podstawie czasu
    for (int i = 0; i < ilosc_losowan; i++)
    {
        x = drand48() * 2;							            //wylosowanie punktu z prostokąta o wymiarach x=2, y=1
        y = drand48();
        if (x < 1)
        {								                        //przedział funkcji x=[0,1]
            if (y <= x) licznik_trafien++;						//sprawdzenie czy punkt znajduje się pod wykresem funkcji y(x)=x
        }
        else
        {										                //przedział funkcji x=[1,2]
            if (y < 2 - x) licznik_trafien++;		            //sprawdzenie czy punkt znajduję się pod wykresem funkcji y(x)=2-x
        }
    }
    p = licznik_trafien / (double) ilosc_losowan;				//obliczenie wartości całki -ilość trafień dzielimy przez liczbę losowań
    p *= 2;											            //następnie mnożymy przez pole powierzchni prostokąta, o wymiarach 2*1=2
    t_end = MPI_Wtime() - t_start;							    //pomiar czasu i obliczenie czasu obliczeń równego róźnicy czasów pomiarów
    MPI_Finalize();


    printf("\n");
    printf("---------------------------------------------------.\n");
    printf("Obliczanie oszacowania wartosci calki oznaczonej\n");
    printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
    printf("---------------------------------------------------.\n");
    printf("Autorzy: Oktawian Kusy, Dominik Mikula\n");
    printf("Grupa: L1/G2 \n");
    printf("---------------------------------------------------.\n");
    printf("Liczba losowan: %d\n", ilosc_losowan);                      //liczba całkowita
    printf("Oszacowanie wartosci calki: %.4f\n", p);                    //liczba zmiennoprzecinkowa, 4 cyfry po przecinku
    printf("Czas obliczen [s]: %.6f \n", t_end);                        //liczba zmiennoprzecinkowa, 6 cyfr po przecinku
    printf("---------------------------------------------------.\n");
    printf("\n");


    FILE* fp = fopen("2_z1KusyMikula.4.lst", "a");			    //zapis wyników do pliku .lst
    if (fp != NULL)
    {
        fprintf(fp, "Liczba losowan: %d, Oszacowanie wartosci calki: %.4f, Czas obliczen [s]: %.6f\n", ilosc_losowan, p, t_end);
        fclose(fp);
    }
    return 0;
}
