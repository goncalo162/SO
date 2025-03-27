#ifndef PARSERCOMANDOS
#define PARSERCOMANDOS

#include "utils.h"


typedef struct comando Comando;

Comando* criaComando(char* argumentos[], int tamanho);

void freeComando(Comando* comando);


#endif