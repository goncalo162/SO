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

void processarComandoCliente(Comando* comandoAtual, const char* ficheirosDir, ServerAuxiliar* serveraux, int fildes[2]) 
{
    int fifoCliente, tipo;
    char nomeFifoCliente[PIPECLIENTE_NOME_MAX];

    snprintf(nomeFifoCliente, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getPidCliente(comandoAtual));

    if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1) //Erro ao abrir o FIFO do cliente
    {
        perror(ERRO_FIFO_CLIENTE_INEXISTENTE);
        _exit(FECHAR);
        unlink(nomeFifoCliente);
    }

    if((tipo = getTipoComando(comandoAtual)) == FECHAR) //Fechar
    {
        Mensagem* mensagemFecho = criaMensagem(-1, false, MENSAGEM_FECHO);
        writeMensagem(mensagemFecho, fifoCliente);
        close(fifoCliente);
        freeMensagem(mensagemFecho);
        _exit(FECHAR);
    }

    Mensagem* outputComando = executaComando(comandoAtual, METADADOS_NOME, strdup(ficheirosDir), serveraux);

    if(isMensagemErro(outputComando)) // Erro
    {
        writeMensagem(outputComando, fifoCliente);
        perrorMensagem(outputComando);
        freeMensagem(outputComando);
        close(fifoCliente);
        _exit(0);
    }

    writeMensagem(outputComando, fifoCliente); // Sucesso
    if(tipo == REMOVER)
    {
        close(fildes[0]);
        int idx = getIndexRelevante(outputComando);
        write(fildes[1], &idx, sizeof(int));
        close(fildes[1]);
    }

    freeMensagem(outputComando);
    close(fifoCliente);
    _exit(0);
}



//* Main

int main(int argc, char* argv[])
{
    if (argc != 3 || !isNumero(argv[2])) 
    {
        perror(ERRO_INPUT_INVALIDO); 
        return 1;
    }

    int fifoServer, estadoGeral = 0;
    ServerAuxiliar* serveraux = initServerAux();

    mkfifo(PIPESERVER_NOME, 0666);
    if ((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
    {
        perror(ERRO_FIFO_SERVIDOR_INEXISTENTE);
        return 1;
    }

    while (estadoGeral != FECHAR) 
    {
        int estadoAtual, n;
        Comando* comandoAtual = readComando(fifoServer, &n);

        if (n > 0 && comandoAtual != NULL)
        {
            int tipo = getTipoComando(comandoAtual);
            int fildes[2];
            pipe(fildes);

            if(tipo == ADICIONAR || tipo == REMOVER || tipo == FECHAR)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    close(fildes[0]);
                    processarComandoCliente(comandoAtual, argv[1], serveraux, fildes);

                }else{
                    close(fildes[1]);
                    freeComando(comandoAtual);
                    waitpid(pid, &estadoAtual, 0);

                    if(WEXITSTATUS(estadoAtual) == FECHAR)
                    {
                        estadoGeral = FECHAR;

                    }else if(tipo == ADICIONAR){
                        popStack(serveraux);

                    }else if(tipo == REMOVER){
                        int idx;
                        if(read(fildes[0], &idx, sizeof(int)) == sizeof(int))
                            pushStack(serveraux, idx);
                    }
                    close(fildes[0]);
                }
            }else{
                pid_t pid = fork();
                close(fildes[0]);
                close(fildes[1]);
                if(pid == 0)
                    processarComandoCliente(comandoAtual, argv[1], serveraux, fildes);
                else
                    freeComando(comandoAtual);
            }
        }
    }

    close(fifoServer);
    unlink(PIPESERVER_NOME);
    freeServerAux(serveraux);
    return 0;
}