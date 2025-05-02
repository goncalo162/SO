#include "comandos.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>



//* Typedef

struct comandoAdicionar
{
    bool removido;
    char titulo[TAMANHO_TITULO], caminho[TAMANHO_PATH];
    char autores[TAMANHO_AUTORES];
    int ano;
};


struct comandoConsulta
{
    int index;
};


struct comandoRemover
{
    int index;
};


struct comandoPesquisaNumLinhas
{
    int index;
    char palavraChave[TAMANHO_PALAVRA_CHAVE];
};


struct comandoPesquisaIds
{
    char palavraChave[TAMANHO_PALAVRA_CHAVE];
};


struct comandoPesquisaIdsMultiproc
{
    char palavraChave[TAMANHO_PALAVRA_CHAVE];
    int numProc;
};


typedef struct comando
{
    TipoComando tipoComando;
    pid_t pidCliente;

    union
    {
        struct comandoAdicionar comandoAdicionar;
        struct comandoConsulta comandoConsulta;
        struct comandoRemover comandoRemover;
        struct comandoPesquisaIds comandoPesquisaIds;
        struct comandoPesquisaNumLinhas comandoPesquisaNumLinhas;
        struct comandoPesquisaIdsMultiproc comandoPesquisaIdsMultiproc;

    } dadosComando;

} Comando;



//* Funções Auxiliares

Comando* criaComandoFechar() 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = FECHAR;
    return nComando;
}


Comando* criaComandoConsulta(int index) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = CONSULTAR;
    nComando->dadosComando.comandoConsulta.index = index;
    return nComando;
}


Comando* criaComandoRemover(int index)  
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = REMOVER;
    nComando->dadosComando.comandoRemover.index = index;
    return nComando;
}


Comando* criaComandoPesquisaIds(char* palavraChave) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = PESQUISA_IDS;
    strncpy(nComando->dadosComando.comandoPesquisaIds.palavraChave, palavraChave, TAMANHO_PALAVRA_CHAVE-1);
    nComando->dadosComando.comandoPesquisaIds.palavraChave[TAMANHO_PALAVRA_CHAVE-1] = '\0';

    return nComando;
}


Comando* criaComandoPesquisaNumLinhas(int index, char* palavraChave) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = PESQUISA_NUM_LINHAS;
    nComando->dadosComando.comandoPesquisaNumLinhas.index = index;
    strncpy(nComando->dadosComando.comandoPesquisaNumLinhas.palavraChave, palavraChave, TAMANHO_PALAVRA_CHAVE-1);
    nComando->dadosComando.comandoPesquisaNumLinhas.palavraChave[TAMANHO_PALAVRA_CHAVE-1] = '\0';
    return nComando;
}


Comando* criaComandoPesquisaIdsMultiproc(char* palavraChave, int numProc) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = PESQUISA_IDS_MULTIPROC;
    strncpy(nComando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave, palavraChave, TAMANHO_PALAVRA_CHAVE-1);
    nComando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave[TAMANHO_PALAVRA_CHAVE-1] = '\0';
    nComando->dadosComando.comandoPesquisaIdsMultiproc.numProc = numProc;
    return nComando;
}
 

Comando* criaComandoAdicionar(char* titulo, char* caminho, char* autores, int ano) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = ADICIONAR;
    nComando->dadosComando.comandoAdicionar.removido = false;
    nComando->dadosComando.comandoAdicionar.ano = ano;
    strncpy(nComando->dadosComando.comandoAdicionar.titulo,titulo, TAMANHO_TITULO-1);
    nComando->dadosComando.comandoAdicionar.titulo[TAMANHO_TITULO-1] = '\0';
    strncpy(nComando->dadosComando.comandoAdicionar.autores,autores, TAMANHO_AUTORES-1);
    nComando->dadosComando.comandoAdicionar.autores[TAMANHO_AUTORES-1] = '\0';
    strncpy(nComando->dadosComando.comandoAdicionar.caminho,caminho, TAMANHO_PATH-1);
    nComando->dadosComando.comandoAdicionar.caminho[TAMANHO_PATH-1] = '\0';

    return nComando;
}



//* Funções de Inicialização

Comando* criaComando(char* argumentos[], int tamanho, pid_t pid) 
{
    Comando* nComando = NULL;
    switch(tamanho) 
    {
        case 1:
            if(strcmp("-f", argumentos[0]) == 0) nComando = criaComandoFechar(); 
            else return NULL;
            break;
        case 2:
            if (strcmp("-c", argumentos[0]) == 0 && isNumero(argumentos[1])) nComando = criaComandoConsulta(atoi(argumentos[1]));
            else if (strcmp("-d", argumentos[0]) == 0 && isNumero(argumentos[1])) nComando = criaComandoRemover(atoi(argumentos[1]));
            else if (strcmp("-s", argumentos[0]) == 0) nComando = criaComandoPesquisaIds(argumentos[1]);
            else return NULL;
            break;
        case 3:
            if (strcmp("-l", argumentos[0]) == 0 && isNumero(argumentos[1])) nComando = criaComandoPesquisaNumLinhas(atoi(argumentos[1]), argumentos[2]);
            else if (strcmp("-s", argumentos[0]) == 0 && isNumero(argumentos[2])) nComando = criaComandoPesquisaIdsMultiproc(argumentos[1], atoi(argumentos[2]));
            else return NULL;
            break;
        case 5:
            if (strcmp("-a", argumentos[0]) == 0 && isNumero(argumentos[3])) nComando = criaComandoAdicionar(argumentos[1], argumentos[4], argumentos[2], atoi(argumentos[3]));
            else return NULL;
            break;
        default:
            return NULL;
    }

    if(nComando != NULL)
        nComando->pidCliente = pid;

    return nComando;
}


Comando* comandoMultiprocParaId(Comando* comando)
{
    Comando* nComando = NULL;
    if(comando->tipoComando != PESQUISA_IDS_MULTIPROC) return NULL;

    nComando = malloc(sizeof(Comando));
    nComando->tipoComando = PESQUISA_IDS;
    strncpy(nComando->dadosComando.comandoPesquisaIds.palavraChave, comando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave, TAMANHO_PALAVRA_CHAVE-1);
    nComando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave[TAMANHO_PALAVRA_CHAVE-1] = '\0';
    return nComando;
}



//* Free

void freeComando(Comando* comando)
{
    free(comando);
}



//* Escrever e ler no ficheiro

int writeComando(int fd, Comando* comando) 
{
    return write(fd, comando, sizeof(Comando));
}


Comando* readComando(int fd, int* n) 
{
    Comando* comando = (Comando*)malloc(sizeof(Comando));
    *n = read(fd, comando, sizeof(Comando));

    if(*n <= 0 || comando->tipoComando == 0) 
    {
        free(comando);
        return NULL;
    }

    return comando;
}



//* Getters

int getDadosComandoAdicionar(Comando* comando, char nomeCopia[TAMANHO_TITULO], char autoresCopia[TAMANHO_AUTORES], char pathCopia[TAMANHO_PATH], int* anoCopia, bool* removidoCopia)
{
    if(comando->tipoComando!=ADICIONAR) return 1;
    
    strncpy(nomeCopia, comando->dadosComando.comandoAdicionar.titulo, TAMANHO_TITULO-1);
    nomeCopia[TAMANHO_TITULO-1] = '\0';
    strncpy(autoresCopia, comando->dadosComando.comandoAdicionar.autores, TAMANHO_AUTORES-1);
    autoresCopia[TAMANHO_AUTORES-1] = '\0';
    strncpy(pathCopia, comando->dadosComando.comandoAdicionar.caminho, TAMANHO_PATH-1);
    pathCopia[TAMANHO_PATH-1] = '\0';
    *anoCopia = comando->dadosComando.comandoAdicionar.ano;
    *removidoCopia = comando->dadosComando.comandoAdicionar.removido;

    return 0;
}


int getIndexComando(Comando* comando)
{
    if(comando->tipoComando == CONSULTAR) return comando->dadosComando.comandoConsulta.index;
    if(comando->tipoComando == REMOVER) return comando->dadosComando.comandoRemover.index;
    if(comando->tipoComando == PESQUISA_NUM_LINHAS) return comando->dadosComando.comandoPesquisaNumLinhas.index;

    return -1;
}


char* getPalavraChaveComando(Comando* comando)
{
    if(comando->tipoComando == PESQUISA_NUM_LINHAS) return strdup(comando->dadosComando.comandoPesquisaNumLinhas.palavraChave);
    if(comando->tipoComando == PESQUISA_IDS) return strdup(comando->dadosComando.comandoPesquisaIds.palavraChave);
    if(comando->tipoComando == PESQUISA_IDS_MULTIPROC) return strdup(comando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave);

    return NULL;
}


int getTipoComando(Comando* comando)
{
    return comando->tipoComando;
}


int getNumProcessos(Comando* comando)
{
    if(comando->tipoComando == PESQUISA_IDS_MULTIPROC) return comando->dadosComando.comandoPesquisaIdsMultiproc.numProc;

    return -1;
}


pid_t getPidCliente(Comando* comando)
{
    return comando->pidCliente;
}