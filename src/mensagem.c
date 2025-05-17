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
    int numPacotes = (tamanho / TAMANHO_PACOTE) + 1; 
    nMensagem->listaPacotes = malloc(sizeof(Pacote) * (numPacotes + 1));
    for(int i=0; i<numPacotes; i++)
    {
        bool isUltimo = false;
        if(i+1 == numPacotes)
            isUltimo = true;

        nMensagem->listaPacotes[i] = criaPacote(isUltimo, info + ((TAMANHO_PACOTE-1) * i)); 
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
    if(!mensagem->erro) 
    {
        for(int i=0; mensagem->listaPacotes[i]; i++)
        {
            writePacote(mensagem->listaPacotes[i], fd);
        }

    }else{ 
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
            if(pacoteAtual->informacao[0] != '\0') 
            {
                numPacotes++;
                resultado = realloc(resultado, TAMANHO_PACOTE * numPacotes);
                strcat(resultado, pacoteAtual->informacao);

            }else{ //Se for pacote de erro
                freePacote(pacoteAtual);
                return NULL;
            }
            if(pacoteAtual->ultimo == true)
            {
                freePacote(pacoteAtual);
                estadoResposta = 1;
            }else{
                freePacote(pacoteAtual);
            }
        }else{
            estadoResposta = 1;
        }
    }

    return resultado;
}



//* Perror

void perrorMensagem(Mensagem* mensagem) {

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