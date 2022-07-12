// Autorzy: Oktawian Kusy, Dominik Mikula, @ Konto: PR1g2
// Cwiczenie 5, Data opracowania 2022-06-30, Wersja 1
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char* argv[])
{
    double t_start, t_end;
    int liczba_komunikatow, ilosc_procesow, id;
    double t3 = 0;
    int ilosc_raportowanych = 10;
    int MASTER = 0, SLAVE = 1;
    char komunikat = 255; // wartość komunikatu - 0xFF
    if (argc != 2 && argc != 3) //sprawdzenie ilosci parametrow
    {
        printf("Zla liczba parametrow: nazwa ilosc_komunikatow ilosc_raportowanych\n");
        return 21;
    }
    liczba_komunikatow = atoi(argv[1]);
    if (liczba_komunikatow < 1)
    {
        printf("Ujemna liczba komunikatow!\n");
        return 22;
    }
    if (argc == 3) ilosc_raportowanych = atoi(argv[2]);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ilosc_procesow);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if (ilosc_procesow > 2) printf("Za duzo procesow!\n\n");
    if (ilosc_procesow < 2) MPI_Abort(MPI_COMM_WORLD, 20);
    if (id == MASTER)
    {
        for (int i = 0; i < liczba_komunikatow; i++)
        {
            t_start = MPI_Wtime();
            if (MPI_Send(&komunikat, 1, MPI_BYTE, 1, 0, MPI_COMM_WORLD) != MPI_SUCCESS)
            {
                printf("ERROR 00");
                MPI_Abort(MPI_COMM_WORLD, 0);
            }
            if (MPI_Recv(&komunikat, 1, MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS)
            {
                printf("ERROR 01");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            t_end = MPI_Wtime() - t_start;
            t_end *= 1000000000; //zamiana na nanosekundy
            if (i < ilosc_raportowanych) printf("RAPORT: cykl %10d, czas trwania %.4f ns.\n", i, t_end);
            t3 += t_end;
        }
        printf("\n");
        printf("Sredni czas komunikatow: %f ns,\nSredni czas przeslania w jedna strone: %f ns\n", t3 / liczba_komunikatow, t3 / (2 * liczba_komunikatow));
    }
    if (id == SLAVE)
    {
        for (int i = 0; i < liczba_komunikatow; i++)
        {
            if (MPI_Recv(&komunikat, 1, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE) != MPI_SUCCESS)
            {
                printf("ERROR 10");
                MPI_Abort(MPI_COMM_WORLD, 10);
            }
            if (MPI_Send(&komunikat, 1, MPI_BYTE, 0, 0, MPI_COMM_WORLD) != MPI_SUCCESS)
            {
                printf("ERROR 11");
                MPI_Abort(MPI_COMM_WORLD, 11);
            }
        }
    }
    MPI_Finalize();
    return 0;
}