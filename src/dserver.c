#include "executor.h"

#define RESPOSTA_TAM_MAX 6000
#define METADADOS_NOME "metadados"
#define PIPESERVER_NOME "pipeServer"
#define PIPECLIENTE_NOME_MAX 60
#define MENSAGEM_FECHO "Server is shuting down"



//* Main

int main(int argc, char* argv[])
{
    if(argc != 3 || !isNumero(argv[2])) 
    {
        perror("Input inválido"); 
        return 1;
    }

    int fifoServer, fifoCliente, estadoGeral = 0;
    char nomeFifoCliente[PIPECLIENTE_NOME_MAX];

    mkfifo(PIPESERVER_NOME, 0666);
    if ((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
    {
        perror("Erro ao abrir FIFO do servidor");
        return 1;
    }

    while(estadoGeral != FECHAR) 
    {
        int estadoAtual, n;
        Comando* comandoAtual = readComando(fifoServer, &n);

        if(n > 0 && comandoAtual != NULL)
        {
            pid_t pid;
            if((pid = fork()) == 0)
            {
                snprintf(nomeFifoCliente, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getPidCliente(comandoAtual));
                if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1) 
                {
                    printf("Erro ao abrir FIFO do cliente");
                    _exit(FECHAR);
                }

                if(getTipoComando(comandoAtual) == FECHAR)
                {
                    write(fifoCliente, MENSAGEM_FECHO, strlen(MENSAGEM_FECHO));
                    _exit(FECHAR);
                }

                char* outputComando = executaComando(comandoAtual, METADADOS_NOME, strdup(argv[1]));

                if(outputComando[0] == '\\') // Erro
                {
                    write(fifoCliente, "!\n", 2);
                    printf("%s", outputComando + 1);
                    close(fifoCliente);
                    _exit(0);
                }

                write(fifoCliente, outputComando, RESPOSTA_TAM_MAX); // Sucesso
                close(fifoCliente);
                _exit(0);

            }else{
                freeComando(comandoAtual);
                wait(&estadoAtual);
                if(WEXITSTATUS(estadoAtual) == 255)
                {    
                    estadoGeral = 255;
                    break;
                }else{
                    continue;
                }
            }
        }    
    }

    close(fifoServer);
    unlink(PIPESERVER_NOME);
    return 0;
}