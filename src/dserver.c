#include "comandos.h"

#define PIPE_NOME "pipe" //Nome que o pipe com nome deverá ter
#define BUFFER 512 //Número máximo em bytes que o servidor poderá ler



//* Main

int main(int argc, char* argv[])
{
    Comando* comandoAtual = criaComandoVazio();
    int fifo, outputComando = 0, n;

    mkfifo(PIPE_NOME, 0666);
    fifo = open(PIPE_NOME, O_RDONLY);

    while(outputComando != -1)
    {
        if(n = read(fifo, comandoAtual, sizeof(Comando*)) > 0)
        {
            outputComando = executaComando(comandoAtual);
        }
    }

    close(fifo);
    unlink(PIPE_NOME);
    freeComando(comandoAtual);
    return 0;
}