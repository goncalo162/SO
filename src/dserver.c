#include "executor.h"

#define RESPOSTA_TAM_MAX 562
#define METADADOS_NOME "metadados"
#define PIPESERVER_NOME "pipeServer"
#define PIPECLIENTE_NOME_MAX 60
#define MENSAGEM_FECHO "Server is shuting down\n\0"



//* Main

int main(int argc, char* argv[])
{
    if(argc != 3 || !isNumero(argv[2])) 
    {
        perror("Input inválido"); 
        return 1;
    }

    Comando* comandoAtual;
    int fifoServer, fifoCliente, estadoGeral = 0;
    char nomeFifoCliente[PIPECLIENTE_NOME_MAX];

    mkfifo(PIPESERVER_NOME, 0666);
    if ((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
    {
        perror("Erro ao abrir FIFO do servidor");
        return 1;
    }

    while(estadoGeral != 255) 
    {
        int estadoAtual;
        pid_t pidAcabou;

        while((pidAcabou = waitpid(-1, &estadoAtual, WNOHANG)) > 0) 
        {
            if (WEXITSTATUS(estadoAtual) == 255) 
            {
                estadoGeral = 255;
                break;
            }
        }

        int n;
        comandoAtual = readComando(fifoServer, &n);

        if(n > 0 && comandoAtual != NULL)
        {
            if (getTipoComando(comandoAtual) == 255) //Confirmar se o comando é fechar, se for saltamos para a próxima iteração do loop e fechar server
            {
                estadoGeral = 255;
                snprintf(nomeFifoCliente, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getPidCliente(comandoAtual));
                if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1) 
                {
                    perror("Erro ao abrir FIFO do cliente");
                    _exit(255);
                }
                write(fifoCliente, MENSAGEM_FECHO, strlen(MENSAGEM_FECHO));
                break;
            }

            pid_t pid;
            if((pid = fork()) == 0)
            {
                snprintf(nomeFifoCliente, PIPECLIENTE_NOME_MAX, "pipeCliente%d", getPidCliente(comandoAtual));
                if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1) 
                {
                    perror("Erro ao abrir FIFO do cliente");
                    _exit(255);
                }

                char* outputComando = executaComando(comandoAtual, METADADOS_NOME, strdup(argv[1]));

                if(outputComando[0] == '\\') // Erro
                {
                    write(fifoCliente, "!\n", 2);
                    printf("%s", outputComando + 1);
                    close(fifoCliente);
                    _exit(0);
                }

                write(fifoCliente, outputComando, strlen(outputComando)); // Sucesso
                close(fifoCliente);
                _exit(0);
            }

        }else{
            close(fifoServer);
            if ((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) 
            {
                perror("Erro ao abrir FIFO do servidor");
                return 1;
            }
        }
    }

    close(fifoServer);
    unlink(PIPESERVER_NOME);
    freeComando(comandoAtual);
    return 0;
}