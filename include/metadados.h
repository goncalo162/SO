#ifndef METADADOS
#define METADADOS

#include "comandos.h"


typedef struct metadados Metadados;


Metadados* criaMetadados(Comando* comando);

void freeMetadados(Metadados* metadados);


bool isRemovido(Metadados* metadados);

char* getPath(Metadados* metadados);

char* getAuthors(Metadados* metadados);

char* getNome(Metadados* metadados);

int getAno(Metadados* metadados);


void setRemovido(Metadados* metadados);


Metadados* readMetadados(int fd);

void writeMetadados(Metadados* metadados, int fd);


#endif