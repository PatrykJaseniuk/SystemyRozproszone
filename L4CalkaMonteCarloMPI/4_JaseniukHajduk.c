// Autorzy: Patryk Jaseniuk ; Franciszek Hajduk; @ Konto: PR1g3 - (dostep z konta PR1g2)
// Cwiczenie: 4, Data opracowania: 2022-05-01, Wersja: 2

#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    // int i;
    // int my_rank;
    int losowaniaProcesu;
    // int p;
    int resztaLosowan;
    int iloscLosowan = 1000;

    // double timeSpend;
    // float calka;
    float xMin = 0;
    float xMax = 2;
    float yMin = 0;
    float yMax = 1.1;
    // float alfa;
    double timeBegin;

    // float pole = obliczPole(xMin, xMax, yMin, yMax);
    float pole = (xMax - xMin) * (yMax - yMin);

    if (argc > 1)
    {
        iloscLosowan = atoi(argv[1]);
    }

    // // MPI_Status status;
    // MPI_Init(&argc, &argv); // inicjalizacja mpi

    // MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pobranie rangi
    // MPI_Comm_size(MPI_COMM_WORLD, &p);       // pobranie liczy procesorow

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // podzial losowan na procesy
    losowaniaProcesu = iloscLosowan / world_size;
    resztaLosowan = iloscLosowan % world_size;

    // pomiar czasu
    timeBegin = MPI_Wtime();

    if (world_rank == 0)
    {
        losowaniaProcesu += resztaLosowan;
    }

    int licznikTrafien = 0;              // niezainicjalizowana zmienna przyjmuje wartosc losowa
    long seed = time(NULL) + world_rank; // ziarna losowania
    srand48(seed);
    for (int i = 0; i < losowaniaProcesu; i++)
    {
        float randX = drand48() * xMax;
        float randY = drand48() * yMax;
        float wynik = sqrt(randX * (2 - randX));
        if (randY < wynik)
        {
            licznikTrafien++;
        }
    }

    // Print off a hello world message
    printf("processor %s, rank %d out of %d processors--> seed: %ld losowaniaProcesu: %d, licznikTrafien: %d\n",
           processor_name, world_rank, world_size, seed, losowaniaProcesu, licznikTrafien);

    // if (my_rank == 0)
    // {
    //     // losowania dla reszty z dzielenia
    //     for (int i = 0; i < resztaLosowan; i++)
    //     {
    //         float randX = drand48() * xMax;
    //         float randY = drand48() * yMax;
    //         float wynik = sqrt(randX * (2 - randX));
    //         if (randY < wynik)
    //         {
    //             licznikTrafien++;
    //         }
    //     }
    // }
    // printf("%d",licznikTrafien);
    // set mpi globa variable
    // int licznikTrafienGlob = licznikTrafien;
    // MPI_Bcast(&licznikTrafienGlob, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // get mpi globa variable
    // MPI_Bcast(&licznikTrafienGlob, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int sumaTrafien = 0;
    MPI_Reduce(&licznikTrafien, &sumaTrafien, 1, MPI_INT, MPI_SUM, 0 /*root*/, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        // obliczenie alfa
        float alfa = (float)sumaTrafien / (float)iloscLosowan;
        // obliczenie przyblizonej wartosci calki
        float calka = alfa * pole;

        double timeSpend = MPI_Wtime() - timeBegin;

        printf("\n");
        printf("---------------------------------------------------.\n");
        printf("Obliczanie oszacowania wartosci calki oznaczonej\n");
        printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
        printf("Wersja rownolegla\n");
        printf("---------------------------------------------------.\n");
        printf("Autorzy: Patryk Jaseniuk Franciszek Hajduk\n");
        printf("Grupa: L1/G1 \n");
        printf("---------------------------------------------------.\n");
        printf("Liczba procesow: %d.\n", world_size);
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
    }

    MPI_Finalize(); // zakonczenie mpi
    return 0;
}