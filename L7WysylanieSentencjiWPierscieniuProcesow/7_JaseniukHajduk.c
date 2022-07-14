#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int SENTENCE_LENGTH = 1000;
int world_size;
int world_rank;
char processor_name[MPI_MAX_PROCESSOR_NAME];

char readLine(char *sentence, int lineNumber)
{
    char czyPrzekopiowanoLinie = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *fp = fopen("sentencje.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (lineNumber == 0)
        {
            strcpy(sentence, line);
            czyPrzekopiowanoLinie = 1;
            break;
        }
        lineNumber--;
    }
    fclose(fp);
    if (line)
        free(line);
    return czyPrzekopiowanoLinie;
}

int sendSentenceToNextTwoProcessors(char *sentence)
{
    int nextProcessor0 = (world_rank + 1) % world_size;
    int nextProcessor1 = (world_rank + 2) % world_size;

    if (nextProcessor0 != 0 && nextProcessor0 != world_rank)
    {

        char status = MPI_Send(sentence, SENTENCE_LENGTH, MPI_CHAR, nextProcessor0, 0, MPI_COMM_WORLD);
        if (status != MPI_SUCCESS)
        {
            // print error message
            printf("Error: MPI_Send failed\n");
        }
        printf("%d: Sending sentence to %d\n", world_rank, nextProcessor0);
    }
    if (nextProcessor1 != 0 && nextProcessor1 != world_rank)
    {
        char status = MPI_Send(sentence, SENTENCE_LENGTH, MPI_CHAR, nextProcessor1, 0, MPI_COMM_WORLD);
        if (status != MPI_SUCCESS)
        {
            // print error message
            printf("Error: MPI_Send failed\n");
        }
        printf("%d: Sending sentence to %d\n", world_rank, nextProcessor1);
    }
}

void addInfoToSentence(char *sentence, int senderRank)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char dataTime[100];
    strftime(dataTime, sizeof(dataTime), "%Y-%m-%d %H:%M:%S", &tm);
    sprintf(sentence, "%s processor: %d otrzymal od: %d data: %s\n", sentence, world_rank, senderRank, dataTime);
}

void reciveSentenceAndSendToMaster(int senderRank)
{
    char sentence[SENTENCE_LENGTH];
    MPI_Recv(sentence, SENTENCE_LENGTH, MPI_CHAR, senderRank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    addInfoToSentence(sentence, senderRank);
    // add processor rank to the sentence

    MPI_Send(sentence, SENTENCE_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    // printf("%s", sentence);
}
void sendAndRecive()
{
    char msg[10] = "Hello\0";
    char msgRecived[10];
    printf("string Length msg: %ld ;msgRecived %ld\n", strlen(msg), strlen(msgRecived));
    MPI_Send(msg, 10, MPI_CHAR, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    MPI_Recv(msgRecived, 10, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("processor: %d %s\n", world_rank, msgRecived);
}

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("👋Hello world from processor %s, rank %d out of %d processors\n", processor_name, world_rank, world_size);

    MPI_Barrier(MPI_COMM_WORLD);

    if (world_rank != 0)
    {
        char sentence[1000] = "Brak Sentencji";

        readLine(sentence, world_rank);
        printf("processor: %d %s\n", world_rank, sentence);

        sendSentenceToNextTwoProcessors(sentence);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("🛑bariera🛑\n");
    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank != 0 && world_size > 2) // zeby proces nie wysylal do siebie samego
    {
        int previousRank = (world_rank - 1);
        if (world_rank == 1)
        {
            previousRank = world_size - 1;
        }
        reciveSentenceAndSendToMaster(previousRank);
        if (world_rank != 1 && world_rank != 2) // proces 1 i 2 dostaja wiadomosc tylko raz
        {
            reciveSentenceAndSendToMaster(previousRank - 1);
        }
    }
    if (world_rank == 0)
    {
        // MPI_Barrier(MPI_COMM_WORLD);
        int iloscWiadomosci = (world_size - 1) * 2 - 2;
        if (world_size <= 2)
        {
            iloscWiadomosci = 0;
        }
        if (world_size == 3)
        {
            iloscWiadomosci = 2;
        }

        for (int i = iloscWiadomosci; i > 0; i--)
        {
            char line[SENTENCE_LENGTH];
            MPI_Recv(line, SENTENCE_LENGTH, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", line);
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}