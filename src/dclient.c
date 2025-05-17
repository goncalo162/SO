#include "comandos.h"
#include "mensagem.h"

#define PIPESERVER_NOME "pipeServer" 
#define PIPECLIENTE_NOME_MAX 50 
#define RESPOSTA_TAM_MAX 200000



//* Main

int main(int argc, char* argv[])
{
    Comando* comando;
    int fifoServer, fifoCliente;
    pid_t pid = getpid();
    char fifoClienteNome[PIPECLIENTE_NOME_MAX];
    snprintf(fifoClienteNome, PIPECLIENTE_NOME_MAX, "pipeCliente%d", pid);
    mkfifo(fifoClienteNome, 0666);

    if((fifoServer = open(PIPESERVER_NOME, O_WRONLY)) == -1) 
    {
        unlink(fifoClienteNome);
        return 1;
    }

    comando = criaComando(argv+1, argc-1, pid);
    if(comando==NULL) 
    {
        close(fifoServer);
        unlink(fifoClienteNome);
        return 1;
    }

    writeComando(fifoServer, comando);
    freeComando(comando);
    close(fifoServer);

    char* resposta = NULL;
    if((fifoCliente = open(fifoClienteNome, O_RDONLY)) == -1)
    {
        unlink(fifoClienteNome);
        return 1;
    }
    
    resposta = readMensagem(fifoCliente);
    if(resposta != NULL)
        printf("%s\n", resposta);
        
    close(fifoCliente);
    unlink(fifoClienteNome);
    return 0;   
}