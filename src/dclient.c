#include "comandos.h"

#define PIPE_NOME "pipe" //Nome que o pipe com nome deverá ter



//* Main

int main(int argc, char* argv[])
{
    Comando* comando;
    int fifo;

    if(!(fifo = open("pipe", O_WRONLY)))
    {
        return 1;
    }


    if(!(comando = criaComando(argv+1, argc-1)))
    {
        return 1;
    }

    write(fifo, comando, sizeof(comando));

    freeComando(comando);
    close(fifo);
    return 0;
}