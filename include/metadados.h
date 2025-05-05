#ifndef METADADOS
#define METADADOS

#include "comandos.h"

#define BUFFER 472 //Tamanho máximo em bytes que metadados pode ter


typedef struct metadados Metadados;


Metadados* criaMetadados(Comando* comando);

void freeMetadados(Metadados* metadados);

void freeMetadadosVoid(void* metadados);

bool isRemovido(Metadados* metadados);

char* getPath(Metadados* metadados);

char* getAuthors(Metadados* metadados);

char* getNome(Metadados* metadados);

int getAno(Metadados* metadados);


void setRemovido(Metadados* metadados);


Metadados* readMetadados(int fd);

void writeMetadados(Metadados* metadados, int fd);


#endif