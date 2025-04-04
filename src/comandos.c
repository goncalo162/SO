#include "comandos.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


//* Typedef

typedef enum tiposComando
{
    ADICIONAR = 1,
    CONSULTAR = 2,
    REMOVER = 3,
    PESQUISA_NUM_LINHAS = 4,
    PESQUISA_IDS = 5,
    PESQUISA_IDS_MULTIPROC = 6,
    FECHAR = 255

} TipoComando;


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
    char* palavraChave;
};


struct comandoPesquisaIds
{
    char* palavraChave;
};


struct comandoPesquisaIdsMultiproc
{
    char* palavraChave;
    int numProc;
};


typedef struct comando
{
    TipoComando tipoComando;

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
    nComando->dadosComando.comandoPesquisaIds.palavraChave = strdup(palavraChave);
    return nComando;
}


Comando* criaComandoPesquisaNumLinhas(int index, char* palavraChave) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = PESQUISA_NUM_LINHAS;
    nComando->dadosComando.comandoPesquisaNumLinhas.index = index;
    nComando->dadosComando.comandoPesquisaNumLinhas.palavraChave = strdup(palavraChave);
    return nComando;
}


Comando* criaComandoPesquisaIdsMultiproc(char* palavraChave, int numProc) 
{
    Comando* nComando = malloc(sizeof(Comando));
    if (!nComando) return NULL;
    nComando->tipoComando = PESQUISA_IDS_MULTIPROC;
    nComando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave = strdup(palavraChave);
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
    strncpy(nComando->dadosComando.comandoAdicionar.titulo,titulo, TAMANHO_TITULO);
    strncpy(nComando->dadosComando.comandoAdicionar.autores,autores, TAMANHO_AUTORES);
    strncpy(nComando->dadosComando.comandoAdicionar.caminho,caminho, TAMANHO_PATH);

    return nComando;
}



//* Funções de Inicialização

Comando* criaComando(char* argumentos[], int tamanho) 
{
    switch (tamanho) 
    {
        case 1:
            if(strcmp("-f", argumentos[0]) == 0) return criaComandoFechar(); 
            return NULL;
        case 2:
            if (strcmp("-c", argumentos[0]) == 0 && isNumero(argumentos[1])) return criaComandoConsulta(atoi(argumentos[1]));
            if (strcmp("-d", argumentos[0]) == 0 && isNumero(argumentos[1])) return criaComandoRemover(atoi(argumentos[1]));
            if (strcmp("-s", argumentos[0]) == 0) return criaComandoPesquisaIds(argumentos[1]);
            return NULL;
        case 3:
            if (strcmp("-l", argumentos[0]) == 0 && isNumero(argumentos[1])) return criaComandoPesquisaNumLinhas(atoi(argumentos[1]), argumentos[2]);
            if (strcmp("-s", argumentos[0]) == 0 && isNumero(argumentos[2])) return criaComandoPesquisaIdsMultiproc(argumentos[1], atoi(argumentos[2]));
            return NULL;
        case 5:
            if (strcmp("-a", argumentos[0]) == 0 && isNumero(argumentos[3]) && ficheiroExiste(argumentos[4])) return criaComandoAdicionar(argumentos[1], argumentos[4], argumentos[2], atoi(argumentos[3]));
            return NULL;
        default:
            return NULL;
    }
}


Comando* criaComandoVazio()
{
    Comando* nComando = malloc(sizeof(Comando));
    return nComando;
}



//* Free

void freeComando(Comando* comando)
{
    if(comando->tipoComando == PESQUISA_NUM_LINHAS)
    {
        free(comando->dadosComando.comandoPesquisaNumLinhas.palavraChave);

    }else if(comando->tipoComando == PESQUISA_IDS){
        free(comando->dadosComando.comandoPesquisaIds.palavraChave);
        
    }else if(comando->tipoComando == PESQUISA_IDS_MULTIPROC){
        free(comando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave);
    }    

    free(comando);
}



//* Escrever e ler no ficheiro

int writeComando(int fd, Comando* comando) 
{
    return write(fd, comando, sizeof(Comando));
}


Comando* readComando(int fd) 
{
    Comando* comando = (Comando*)malloc(sizeof(Comando));
    read(fd, comando, sizeof(Comando));

    return comando;
}



//* Getters

int getDadosComandoAdicionar(Comando* comando, char nomeCopia[TAMANHO_TITULO], char autoresCopia[TAMANHO_AUTORES], char pathCopia[TAMANHO_PATH], int* anoCopia, bool* removidoCopia)
{
    if(comando->tipoComando!=ADICIONAR) return 1;
    
    strncpy(nomeCopia, comando->dadosComando.comandoAdicionar.titulo, TAMANHO_TITULO);
    strncpy(autoresCopia, comando->dadosComando.comandoAdicionar.autores, TAMANHO_AUTORES);
    strncpy(pathCopia, comando->dadosComando.comandoAdicionar.caminho, TAMANHO_PATH);
    *anoCopia = comando->dadosComando.comandoAdicionar.ano;
    *removidoCopia = comando->dadosComando.comandoAdicionar.removido;

    return 0;
}


int getIndexComando(Comando* comando)
{
    if(comando->tipoComando == CONSULTAR) return comando->dadosComando.comandoConsulta.index;
    if(comando->tipoComando == REMOVER) return comando->dadosComando.comandoRemover.index;

    return -1;
}


int getTipoComando(Comando* comando)
{
    return comando->tipoComando;
}