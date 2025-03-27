#include "comandos.h"


//* Main

int main(int argc, char* argv[])
{
    Comando* comando;

    if(!(comando = criaComando(argv+1, argc)))
    {
        return 1;
    }

    freeComando(comando);
    return 0;
}