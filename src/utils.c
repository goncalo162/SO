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


bool ficheiroExiste(const char* caminho)
{
    struct stat buffer;
    if(stat(caminho, &buffer) == -1)
    {
        return false;
    }
    return true;
}


bool ficheiroVazio(const char* caminho)
{
    struct stat buffer;
    if(stat(caminho, &buffer) == 0)
    {
        return buffer.st_size == 0; //Retorna true se o tamanho do ficheiro não for 0 bytes
    }
    return true;
}