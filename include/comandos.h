#ifndef PARSERCOMANDOS
#define PARSERCOMANDOS

#include "utils.h"


typedef struct comando Comando;

Comando* criaComando(char* argumentos[], int tamanho);

Comando* criaComandoVazio();

void freeComando(Comando* comando);

int executaComando(Comando* comando);


#endif