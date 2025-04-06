#ifndef EXECUTOR
#define EXECUTOR

#include "serveraux.h"

typedef struct metadados Metadados;

//Recebe um comando e um caminho para o ficheiro de metadados e executa o comando
char* executaComando(Comando* comando, char* caminhoMetadados, char* ficheirosDir);


#endif