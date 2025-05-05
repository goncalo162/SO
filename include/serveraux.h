#ifndef SERVERAUX
#define SERVERAUX

#include "metadados.h"

typedef struct serverAux ServerAuxiliar;

ServerAuxiliar* initServerAux();

int popStack(ServerAuxiliar* stack);

void pushStack(ServerAuxiliar* stack, int index);

bool isStackEmpty(ServerAuxiliar* stack);

void freeServerAux(ServerAuxiliar* stack);

int getIndexCabeca(ServerAuxiliar* stack);

void debugPrintStack(ServerAuxiliar *stack);

#endif