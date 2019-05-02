#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <sys/time.h>
#include <omp.h>

// NUMERO MAXIMO DE CARACTERES DA SENHA
#define MAX_CHARS 4

// PROTOTIPOS
void TimeInit(void);
double TimeStart(void);
double TimeStop(double);

// VALOR DO OVERHEAD DA MEDICAO DE TEMPO
static double TimeOverhead = 0.0;

// CARACTERES VALIDOS
char *valid_chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
// SENHA QUE SE DESEJA ENCONTRAR
char *hash_to_search = "aaI8pRQwCn7N2";
int num_valid_chars;
char salt[3];


// FUNCAO QUE CALCULA O OVERHEAD DA MEDICAO DE TEMPO
void TimeInit()
{
    double t;

    TimeOverhead = 0.0;
    t = TimeStart();
    TimeOverhead = TimeStop(t);
}

// FUNCAO QUE CAPTURA O TEMPO INICIAL DO TRECHO A SER MEDIDO
double TimeStart()
{
    struct timeval tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) != 0)
        exit(1);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// FUNCAO QUE CALCULA O TEMPO GASTO NO FINAL DO TRECHO A SER MEDIDO
double TimeStop(double TimeInitial)
{
    struct timeval tv;
    struct timezone tz;
    double Time;

    if (gettimeofday(&tv, &tz) != 0)
        exit(1);
    Time = tv.tv_sec + tv.tv_usec / 1000000.0;
    return Time - TimeInitial - TimeOverhead;
}

// FUNCAO QUE SERA CHAMADA RECURSIVAMENTE 
// (teóricamente privada, pois só deve ser chamada dentro da função dive original)
void _dive(char prefix[], int level, struct crypt_data *data)
{

    level += 1;
    for (int i = 0; i < num_valid_chars; i++)
    {
        if (level <= MAX_CHARS)
        {
			prefix[(level-1)] = valid_chars[i];
			prefix[level] =  '\0';

			if (strcmp(hash_to_search,crypt_r(prefix,salt, data))==0){
				fprintf(stderr,"Found password=%s\n",prefix);
			}
            _dive(prefix, level, data);
        }
    }
}

void dive()
{
    //Gera threads que serão responsáveis por executar um galho da árvore de palavras
    //Cada thread é responsável por uma um prefíxo (token) inicial
    #pragma omp parallel for
    for (int i = 0; i < num_valid_chars; i++)
    {	
	struct crypt_data data;
	data.initialized = 0;
	int currentLevel = 1;
	    
	//Condição de parada, ao ultrapassar o 
	//tamanho máximo da palavra (controlado pela variável level) para de executar
        if (currentLevel <= MAX_CHARS)
        {
			char newPrefix[MAX_CHARS + 1];
			newPrefix[currentLevel-1] = valid_chars[i];
			newPrefix[currentLevel] =  '\0';
			if (strcmp(hash_to_search,crypt_r(newPrefix,salt,&data))==0){
				fprintf(stderr,"Found password=%s\n",newPrefix);
			}
            _dive(newPrefix, currentLevel, &data);
        }
    }
}



int main()
{
    int num_chars, i, j, pos, vai_um;
    char indChars[MAX_CHARS];
    double inicio, total;

	num_valid_chars = strlen(valid_chars);

    TimeInit();
    inicio = TimeStart();

    salt[0] = hash_to_search[0];
    salt[1] = hash_to_search[1];
    salt[2] = '\0';

    dive();

    // MOSTRA O TEMPO DE EXECUCAO
    total = TimeStop(inicio);
    printf("%lf", total);
    return 0;
}
