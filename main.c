// sequencial.c (Roland Teodorowitsch; 4 abr. 2019)

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

#define MAX_CHARS 4

// PROTOTIPOS
void	TimeInit   (void);
double	TimeStart  (void);
double	TimeStop   (double);

// VALOR DO OVERHEAD DA MEDICAO DE TEMPO
static double TimeOverhead=0.0;

// FUNCAO QUE CALCULA O OVERHEAD DA MEDICAO DE TEMPO
void TimeInit()
{
  double t;

  TimeOverhead=0.0;
  t = TimeStart();
  TimeOverhead=TimeStop(t);
}

// FUNCAO QUE CAPTURA O TEMPO INICIAL DO TRECHO A SER MEDIDO
double TimeStart()
{
  struct timeval tv;
  struct timezone tz;

  if (gettimeofday(&tv,&tz)!=0)
      exit(1);
  return tv.tv_sec + tv.tv_usec/1000000.0;
}

// FUNCAO QUE CALCULA O TEMPO GASTO NO FINAL DO TRECHO A SER MEDIDO
double TimeStop(double TimeInitial)
{
  struct timeval tv;
  struct timezone tz;
  double Time;

  if (gettimeofday(&tv,&tz)!=0)
      exit(1);
  Time = tv.tv_sec + tv.tv_usec/1000000.0;
  return Time-TimeInitial-TimeOverhead;
}

int main() {
  int num_chars,num_valid_chars,i,j,pos, vai_um;
  char *valid_chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
  char *hash_to_search = "aaI8pRQwCn7N2";
  char salt[3];
  char key[MAX_CHARS+1];
  char indChars[MAX_CHARS];
  struct crypt_data data;
  double inicio, total;

  TimeInit();
  inicio = TimeStart(); 

  data.initialized = 0;

  salt[0] = hash_to_search[0];
  salt[1] = hash_to_search[1];
  salt[2] = '\0';

  num_valid_chars = strlen(valid_chars);
  // from 1 to MAX_CHARS characters in a key
  for (num_chars=1; num_chars<=MAX_CHARS; ++num_chars) {

      // Initial key
      for (i=0; i<num_chars; ++i)
          indChars[i] = 0;

      do {

        // Generates a key
        for (j=0;j<num_chars; ++j)
          key[j] = valid_chars[indChars[j]];
        key[j] = '\0';

        // Is this key the encrypted password?
        if (strcmp(hash_to_search,crypt_r(key,salt,&data))==0)
           fprintf(stderr,"Found password=%s\n",key);

        // Determines the indexes for the next key
        pos = num_chars-1;
        vai_um = 1;
        while (pos >= 0 && vai_um) {
              indChars[pos]++;
              // if (pos==0) { printf("."); fflush(stdout); }
              if (indChars[pos]==num_valid_chars) {
                 indChars[pos] = 0; 
                 --pos;
              }
              else
                 vai_um = 0;
        }

      } while (pos >= 0);

      // printf("\n");
  }

  // MOSTRA O TEMPO DE EXECUCAO
  total = TimeStop(inicio);
  printf("%lf",total);
  return 0;
}