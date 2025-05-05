#ifndef MENSAGEM
#define MENSAGEM

#define TAMANHO_PACOTE 4000 //Número em bytes máximo que um pacote pode transportar

#include "utils.h"


typedef struct mensagem Mensagem;

Mensagem* criaMensagem(int valorIndex, bool isErro, char* info);

void freeMensagem(Mensagem* mensagem);

void writeMensagem(Mensagem* mensagem, int fd);

char* readMensagem(int fd);

void perrorMensagem(Mensagem* mensagem);

int getIndexRelevante(Mensagem* mensagem);

bool isMensagemErro(Mensagem* mensagem);


#endif