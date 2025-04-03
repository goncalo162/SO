#include "executor.h"

#define RESPOSTA_TAM_MAX 600 //Número máximo de bytes que uma resposta pode ter
#define METADADOS_NOME "metadados" //Nome que o ficheiro com os metadados deverá ter 
#define PIPESERVER_NOME "pipeServer" //Nome que o pipe com nome para enviar mensages ao servidor deverá ter
#define PIPECLIENTE_NOME_MAX 60 //Número máximo de bytes que o nome do pipe com nome para receber mensagens do servidor deverá ter



//* Main

int main(int argc, char* argv[])
{
    Comando* comandoAtual = criaComandoVazio();
    int fifoServer, fifoCliente, n, estadoGeral = 0;
    char* outputComando, nomeFifoCliente[PIPECLIENTE_NOME_MAX];

    mkfifo(PIPESERVER_NOME, 0666);
    if((fifoServer = open(PIPESERVER_NOME, O_RDONLY)) == -1) return 1;

    while(estadoGeral != 255)
    {
        int estadoAtual; // Antes de ler comandos novos confirmamos se nenhum dos processos filho retornou o código de saída de FECHAR
        pid_t pidAcabou;
        while ((pidAcabou = waitpid(-1, &estadoAtual, WNOHANG)) > 0)
        {
            if(WEXITSTATUS(estadoAtual) == 255)
            {
                estadoGeral = 255;
                break;  // Acabar com este while imediatamente para fechar logo o programa se detetarmos que o utilizar passou o comando FECHAR
            }
        }

        if((read(fifoServer, nomeFifoCliente, PIPECLIENTE_NOME_MAX)) > 0)
        {
            comandoAtual = readComando(fifoServer);//Se nenhum dos comandos anteriores foi FECHAR então podemos ler o próximo comando
            
            pid_t pid;
            if((pid = fork())==0)
            {
                outputComando = executaComando(comandoAtual, METADADOS_NOME); 
                if((fifoCliente = open(nomeFifoCliente, O_WRONLY)) == -1)
                {
                    printf("ERRO! Não foi possivel abrir %s \n", nomeFifoCliente);
                    _exit(255);
                }

                if(!outputComando) //Fechar
                {
                    printf("Fechar\n");
                    write(fifoCliente, "!\0", RESPOSTA_TAM_MAX);
                    _exit(255);
                }

                if(outputComando[0] == '\\') //Erro
                {
                    printf("%s", outputComando+1);
                    write(fifoCliente, "!\0", RESPOSTA_TAM_MAX);
                    _exit(0);
                }
                write(fifoCliente, outputComando, strlen(outputComando)); //Comando bem sucedido
                close(fifoCliente);
                _exit(0);
            }
        }
    }

    close(fifoServer);
    unlink(PIPESERVER_NOME);
    freeComando(comandoAtual);
    return 0;
}