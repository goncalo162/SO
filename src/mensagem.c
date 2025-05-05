#include "mensagem.h"



//* Typedef

typedef struct pacote
{
    bool ultimo;
    char informacao[TAMANHO_PACOTE];

} Pacote;


typedef struct mensagem
{
    Pacote** listaPacotes;
    bool erro;
    int indexRelevante;

} Mensagem;



//* Funções de Inicialização

Pacote* criaPacote(bool isUltimo, char* info)
{
    Pacote* nPacote = malloc(sizeof(Pacote));

    if(info)
    {
        strncpy(nPacote->informacao, info, TAMANHO_PACOTE-1);
        nPacote->informacao[TAMANHO_PACOTE-1] = '\0';

    }else{
        nPacote->informacao[0] = '\0';
    }

    nPacote->ultimo = isUltimo;
    return nPacote;
}


Mensagem* criaMensagem(int valorIndex, bool isErro, char* info)
{
    Mensagem* nMensagem = malloc(sizeof(Mensagem));
    int tamanho = strlen(info);
    int numPacotes = (tamanho / TAMANHO_PACOTE) + 1; //Divisão inteira, por isso adicionamos '1' ao resultado

    nMensagem->listaPacotes = malloc(sizeof(Pacote) * (numPacotes + 1));
    for(int i=0; i<numPacotes; i++)
    {
        bool isUltimo = false;
        if(i+1 == numPacotes)
            isUltimo = true;

        nMensagem->listaPacotes[i] = criaPacote(isUltimo, info + ((TAMANHO_PACOTE-1) * i)); //Começar a ler a informação somando i vezes o tamanho do pacote
    }
    nMensagem->listaPacotes[numPacotes] = NULL; 
    nMensagem->indexRelevante = valorIndex;
    nMensagem->erro = isErro;
    return nMensagem;
}



//* Free

void freePacote(Pacote* pacote)
{
    free(pacote);
}


void freeMensagem(Mensagem* mensagem)
{
    for(int i=0; mensagem->listaPacotes[i]; i++)
    {
        free(mensagem->listaPacotes[i]);
    }
    free(mensagem->listaPacotes);
    free(mensagem);
}



//* Write

void writePacote(Pacote* pacote, int fd)
{
    write(fd, pacote, sizeof(Pacote));
}


void writeMensagem(Mensagem* mensagem, int fd)
{
    if(!mensagem->erro) //Sem Erro
    {
        for(int i=0; mensagem->listaPacotes[i]; i++)
        {
            writePacote(mensagem->listaPacotes[i], fd);
        }

    }else{ //Erro
        Pacote* pacoteErro = criaPacote(true, NULL);
        writePacote(pacoteErro, fd);
    }
}



//* Read

Pacote* readPacote(int fd)
{
    Pacote* nPacote = malloc(sizeof(Pacote));
    int n = 0;

    if((n = read(fd, nPacote, sizeof(Pacote))) <= 0)
    {
        free(nPacote);
        return NULL;
    }

    return nPacote;
}


char* readMensagem(int fd)
{
    char* resultado = malloc(sizeof(char));
    int estadoResposta = 0, numPacotes = 0;
    Pacote* pacoteAtual = NULL;

    while(estadoResposta != 1)
    {
        pacoteAtual = readPacote(fd);
        if(pacoteAtual != NULL)
        {
            if(pacoteAtual->informacao[0] != '\0') //Verifica se não é um pacote de erro
            {
                numPacotes++;
                resultado = realloc(resultado, TAMANHO_PACOTE * numPacotes);
                strcat(resultado, pacoteAtual->informacao);

            }else{ //Se for pacote de erro
                freePacote(pacoteAtual);
                return NULL;
            }
        }

        if(pacoteAtual->ultimo == true)
        {
            freePacote(pacoteAtual);
            estadoResposta = 1;
        }else{
            freePacote(pacoteAtual);
        }
    }

    return resultado;
}



//* Perror

void perrorMensagem(Mensagem* mensagem) //Assume que todas as mensagens de erro necessitam apenas de um pacote
{
    perror(mensagem->listaPacotes[0]->informacao);
}



//* Getters

int getIndexRelevante(Mensagem* mensagem)
{
    return mensagem->indexRelevante;
}


bool isMensagemErro(Mensagem* mensagem)
{
    return mensagem->erro;
}