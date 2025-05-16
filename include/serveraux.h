#ifndef SERVERAUX
#define SERVERAUX

#include "metadados.h"
#include "cache.h"


typedef struct serverAux ServerAuxiliar;

ServerAuxiliar* initServerAux();

int popStack(ServerAuxiliar* stack);

void pushStack(ServerAuxiliar* stack, int index);

bool isStackEmpty(ServerAuxiliar* stack);

void freeServerAux(ServerAuxiliar* stack);

int getIndexCabeca(ServerAuxiliar* stack);

void debugPrintStack(ServerAuxiliar *stack);


Metadados* serveraux_get_cache(ServerAuxiliar* aux, int index);
bool serveraux_has_cache(ServerAuxiliar* aux, int index);
void serveraux_put_cache(ServerAuxiliar* aux, int index, Metadados* meta);
Cache* getCache(ServerAuxiliar* aux);


#endif 