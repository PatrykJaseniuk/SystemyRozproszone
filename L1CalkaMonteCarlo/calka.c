
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

char *odpowiedz(int iloscLosowan, float wynikCalki, int czasObliczen)
{
  char text[20];
  char *odpowiedz = (char *)malloc(10000 * sizeof(char));
  strcat(odpowiedz, "\n");
  strcat(odpowiedz, "Obliczanie oszacowania wartości calki oznaczonej\n");
  strcat(odpowiedz, "z funkcji y=sqrt(x(2-x)) metoda Monte Carlo\n");
  strcat(odpowiedz, "---------------------------------------------------.\n");
  strcat(odpowiedz, "Autor(-rzy): Patryk Jaseniuk, Franciszek Hajduk");
  strcat(odpowiedz, "Grupa: L1/G3\n");
  strcat(odpowiedz, "---------------------------------------------------\n");
  strcat(odpowiedz, "Liczba losowan: ");
  sprintf(text, "%d", iloscLosowan);
  strcat(odpowiedz, text);

  strcat(odpowiedz, "\nOszacowanie wartości calki: ");
  gcvt(wynikCalki, 4, text);
  strcat(odpowiedz, text);
  strcat(odpowiedz, "\nCzas obliczeń [s]: ");
  sprintf(text, "%d", czasObliczen);
  strcat(odpowiedz, text);
  strcat(odpowiedz, "\n");
  return odpowiedz;
}

float obliczPole(float xMin, float xMax, float yMin, float yMax) { return (xMax - xMin) * (yMax - yMin); }

float funkcja(float x)
{
  return sqrt(x * (2 - x));
}

float obliczAlfa(float xMin, float xMax, float yMin, float yMax, int iloscLosowan)
{
  float seed = 12474836594623027;
  int licznikTrafien;
  srand48(seed);
  for (int i = 0; i < iloscLosowan; i++)
  {
    float randX = drand48() * xMax;
    float randY = drand48() * yMax;

    if (randY < funkcja(randX))
    {
      licznikTrafien++;
    }
  }
  return (float)licznikTrafien / (float)iloscLosowan;
}

float obliczCalke(iloscLosowan)
{
  float xMin = 0;
  float xMax = 2;
  float yMin = 0;
  float yMax = 1.1;

  float alfa = obliczAlfa(xMin, xMax, yMin, yMax, iloscLosowan);
  float pole = obliczPole(xMin, xMax, yMin, yMax);
  float calka = alfa * pole;
  return calka;
}

void zapiszDoPliku(char *odpowiedx)
{
  FILE *f = fopen("calka.lst", "a");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  /* print some text */
  // const char *text = "Write this to the file";
  fprintf(f, "%s\n\n", odpowiedx);
}

int main(int argC, char **arg)
{
  int iloscLosowan = 1000;
  if (argC > 1)
  {
    iloscLosowan = atoi(arg[1]);
  }

  // clock_t begin = clock();
  float calka = obliczCalke(iloscLosowan);
  // clock_t end = clock();
  // int time_spent = (int)((end - begin) / CLOCKS_PER_SEC);

  char *odpowiedx = odpowiedz(iloscLosowan, calka, 2);
  printf("%s", odpowiedx);
  // printf(arg[1]);
  zapiszDoPliku(odpowiedx);

  return 0;
}
