#ifndef EXECUTOR
#define EXECUTOR

#include "serveraux.h"
#include "mensagem.h"

typedef struct metadados Metadados;

//Recebe um comando e um caminho para o ficheiro de metadados e executa o comando
Mensagem* executaComando(Comando* comando, char* caminhoMetadados, char* ficheirosDir);


#endif