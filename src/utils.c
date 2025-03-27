#include "utils.h"



//* Funções de Utilidade

bool isNumero(char* texto)
{
    for(int i=0; texto[i]!='\0'; i++)
    {
        if(texto[i] < '0' || texto[i] > '9')
            return false;
    }

    return true;
}