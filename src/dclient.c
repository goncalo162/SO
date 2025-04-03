#include "comandos.h"

#define PIPESERVER_NOME "pipeServer" //Nome que o pipe com nome para enviar mensagens aos servidor deverá ter
#define PIPECLIENTE_NOME_MAX 50 //Número máximo de bytes que o nome do pipe com nome para receber mensagens do servidor deverá ter
#define RESPOSTA_TAM_MAX 600 //Número máximo de bytes que uma resposta pode ter


//* Main

int main(int argc, char* argv[])
{
    Comando* comando;
    int fifoServer, fifoCliente, estadoResposta = 0;

    char fifoClienteNome[PIPECLIENTE_NOME_MAX];
    snprintf(fifoClienteNome, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getpid());
    mkfifo(fifoClienteNome, 0666);

    if((fifoServer = open("pipeServer", O_WRONLY)) == -1) return 1;
    if((fifoCliente = open(fifoClienteNome, O_RDONLY|O_NONBLOCK)) == -1) return 1;

    write(fifoServer, fifoClienteNome, PIPECLIENTE_NOME_MAX);
    comando = criaComando(argv+1, argc-1);
    writeComando(fifoServer, comando);
    freeComando(comando);
    close(fifoServer);

    char resposta[RESPOSTA_TAM_MAX];
    int n;

    while(estadoResposta != 1)
    {
        n = read(fifoCliente, resposta, RESPOSTA_TAM_MAX);

        if(n>0 || resposta == NULL)
            estadoResposta = 1;
    }

    if(resposta[0]!='!')
        printf("%s", resposta);
        
    close(fifoCliente);
    unlink(fifoClienteNome);
    return 0;   
}