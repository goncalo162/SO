#include "executor.h"

#define RESPOSTA_TAM_MAX 200000
#define METADADOS_NOME "metadados"
#define PIPESERVER_NOME "pipeServer"
#define PIPECLIENTE_NOME_MAX 60
#define MENSAGEM_FECHO "Server is shuting down"
#define ERRO_FIFO_CLIENTE_INEXISTENTE "ERRO! Não foi possível abrir o FIFO do cliente\n\0"
#define ERRO_FIFO_SERVIDOR_INEXISTENTE "ERRO! Não foi possível abrir o FIFO do servidor\n\0"



//* Processar Comandos

void processarComandoCliente(Comando* comandoAtual, const char* ficheirosDir) 
{
    int fifoCliente;
    char nomeFifoCliente[PIPECLIENTE_NOME_MAX];

    snprintf(nomeFifoCliente, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getPidCliente(comandoAtual));

    if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1) //Erro ao abrir o FIFO do cliente
    {
        perror(ERRO_FIFO_CLIENTE_INEXISTENTE);
        _exit(FECHAR);
        unlink(nomeFifoCliente);
    }

    if (getTipoComando(comandoAtual) == FECHAR) //Fechar
    {
        write(fifoCliente, MENSAGEM_FECHO, strlen(MENSAGEM_FECHO));
        close(fifoCliente);
        _exit(FECHAR);
    }

    char* outputComando = executaComando(comandoAtual, METADADOS_NOME, strdup(ficheirosDir));

    if (outputComando[0] == '\\') // Erro
    {
        write(fifoCliente, "!\n", 2);
        perror(outputComando + 1);
        free(outputComando);
        close(fifoCliente);
        _exit(0);
    }

    write(fifoCliente, outputComando, RESPOSTA_TAM_MAX);  // Sucesso
    free(outputComando);
    close(fifoCliente);
    _exit(0);
}



//* Main

int main(int argc, char* argv[])
{
    if(argc != 3 || !isNumero(argv[2])) 
    {
        perror("Input inválido"); 
        return 1;
    }

    int fifoServer, estadoGeral = 0;

    mkfifo(PIPESERVER_NOME, 0666);
    if ((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
    {
        printf("%s", ERRO_FIFO_SERVIDOR_INEXISTENTE);
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
                processarComandoCliente(comandoAtual, argv[1]);

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