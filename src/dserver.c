#include "executor.h"

#define RESPOSTA_TAM_MAX 200000
#define METADADOS_NOME "metadados"
#define PIPESERVER_NOME "pipeServer"
#define PIPECLIENTE_NOME_MAX 60
#define MENSAGEM_FECHO "Server is shuting down"
#define ERRO_FIFO_CLIENTE_INEXISTENTE "ERRO! Não foi possível abrir o FIFO do cliente"
#define ERRO_FIFO_SERVIDOR_INEXISTENTE "ERRO! Não foi possível abrir o FIFO do servidor"
#define ERRO_INPUT_INVALIDO "ERRO! Input inválido"



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

    if(getTipoComando(comandoAtual) == FECHAR) //Fechar
    {
        Mensagem* mensagemFecho = criaMensagem(-1, false, MENSAGEM_FECHO);
        writeMensagem(mensagemFecho, fifoCliente);
        close(fifoCliente);
        freeMensagem(mensagemFecho);
        _exit(FECHAR);
    }

    Mensagem* outputComando = executaComando(comandoAtual, METADADOS_NOME, strdup(ficheirosDir));

    if(isMensagemErro(outputComando)) // Erro
    {
        writeMensagem(outputComando, fifoCliente);
        perrorMensagem(outputComando);
        freeMensagem(outputComando);
        close(fifoCliente);
        _exit(0);
    }

    writeMensagem(outputComando, fifoCliente);  // Sucesso
    freeMensagem(outputComando);
    close(fifoCliente);
    _exit(0);
}



//* Main

int main(int argc, char* argv[])
{
    if(argc != 3 || !isNumero(argv[2])) 
    {
        perror(ERRO_INPUT_INVALIDO); 
        return 1;
    }

    int fifoServer, estadoGeral = 0;

    mkfifo(PIPESERVER_NOME, 0666);
    if((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
    {
        perror(ERRO_FIFO_SERVIDOR_INEXISTENTE);
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