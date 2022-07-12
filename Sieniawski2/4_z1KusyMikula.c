// Autorzy: Oktawian Kusy; Dominik Mikula; @ Konto: PR1g2
// Cwiczenie: 4, Data opracowania: 2022-06-28, Wersja: 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define srand48(x) srand((int)(x))
#define drand48() ((double)rand()/RAND_MAX)
int main(int argc, char* argv[])
{
    int rank, size, liczba_losowan;
    int wynik = 0;
    int master = 0;
    MPI_Comm comm;
    comm = MPI_COMM_WORLD;
    MPI_Init(&argc, &argv);                                                             //inicjalizacja MPI
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    double x, y, calka, t_start, t_end;
    int licznik_trafien = 0;
    if (argv[1] != "") { liczba_losowan = atoi(argv[1]); }
    else { liczba_losowan = 1000; }
    t_start = MPI_Wtime();
    srand48(t_start);
    for (int i = 0; i < liczba_losowan / size; i++) {
        x = drand48() * 2;
        y = drand48();
        if (y < sqrt(x*( 2 - x))) licznik_trafien++;
    }
    if (rank == size -1) {
        for (int i = 0; i < liczba_losowan % size; i++) {
            x = drand48() * 2;
            y = drand48();
            if (y < sqrt(x*(2 - x))) licznik_trafien++;
        }
    }
    MPI_Reduce(&licznik_trafien, &wynik, 1, MPI_INT, MPI_SUM, master, comm);  //uzycie MPI_Reduce wymusza obliczenia na procesie glownym
    if (rank == 0) {
        calka = wynik / (double) liczba_losowan;
        calka *= 2;
        t_end = MPI_Wtime() - t_start;
        printf("\n");
        printf("Obliczanie oszacowania wartosci calki oznaczonej \n");
        printf("z funkcji y=sqrt(x(2-x)) metoda Monte Carlo. \n");
        printf("Wersja rownolegla \n");
        printf("----------------------------------------------------------. \n");
        printf("Autorzy: Oktawian Kusy, Dominik Mikula \n");
        printf("Grupa: PR1g2 \n");
        printf("----------------------------------------------------------. \n");
        printf("Liczba procesow: %d\n", size);                                                       //liczba calkowita
        printf("Liczba losowan: %d\n", liczba_losowan);                                              //liczba calkowita
        printf("Oszacowanie wartosci calki: %.4f\n", calka);                                         //liczba zmiennoprzecinkowa, 4 cyfry po przecinku
        printf("Czas obliczen [s]: %.6f \n", t_end);                                                 //liczba zmiennoprzecinkowa, 6 cyfr po przecinku
        printf("\n");
    }
    MPI_Finalize();
}