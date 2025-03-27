#include "comandos.h"



//* Typedef

typedef enum comandos
{
    ADICIONAR,
    CONSULTAR,
    REMOVER,
    PESQUISA_NUM_LINHAS,
    PESQUISA_IDS,
    PESQUISA_IDS_MULTIPROC,
    FECHAR

} Comandos;


struct comandoAdicionar
{
    char* titulo, *caminho;
    char** autores;
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
    Comandos tipoComando;

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
    nComando->dadosComando.comandoAdicionar.ano = ano;
    nComando->dadosComando.comandoAdicionar.titulo = strdup(titulo);
    nComando->dadosComando.comandoAdicionar.caminho = strdup(caminho);

    char* atual, *copia = strdup(autores);
    char* inicio = copia;
    int numAutores = 0;
    while ((atual = strsep(&copia, ","))) {
        numAutores++;
        nComando->dadosComando.comandoAdicionar.autores = realloc(nComando->dadosComando.comandoAdicionar.autores, sizeof(char*) * numAutores);
        if (!nComando->dadosComando.comandoAdicionar.autores) {
            free(nComando);
            free(inicio);
            return NULL;
        }
        nComando->dadosComando.comandoAdicionar.autores[numAutores - 1] = strdup(atual);
    }
    free(inicio);
    return nComando;
}



//* Funções de Inicialização

Comando* criaComando(char* argumentos[], int tamanho) 
{
    switch (tamanho) {
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
            if (strcmp("-a", argumentos[0]) == 0 && isNumero(argumentos[3])) return criaComandoAdicionar(argumentos[1], argumentos[4], argumentos[2], atoi(argumentos[3]));
            return NULL;
        default:
            return NULL;
    }
}



//* Funções de Remoção

void freeComando(Comando* comando)
{
    if(comando->tipoComando == ADICIONAR)
    {
        for(int i=0; comando->dadosComando.comandoAdicionar.autores[i]; i++)
            free(comando->dadosComando.comandoAdicionar.autores[i]);
        free(comando->dadosComando.comandoAdicionar.autores);

        free(comando->dadosComando.comandoAdicionar.caminho);
        free(comando->dadosComando.comandoAdicionar.titulo);

    }else if(comando->tipoComando == PESQUISA_NUM_LINHAS){
        free(comando->dadosComando.comandoPesquisaNumLinhas.palavraChave);

    }else if(comando->tipoComando == PESQUISA_IDS){
        free(comando->dadosComando.comandoPesquisaIds.palavraChave);
        
    }else if(comando->tipoComando == PESQUISA_IDS_MULTIPROC){
        free(comando->dadosComando.comandoPesquisaIdsMultiproc.palavraChave);
    }    

    free(comando);
}