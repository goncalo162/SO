#include "metadados.h"



//* Typedef

typedef struct metadados
{
    char nome[TAMANHO_TITULO], autor[TAMANHO_AUTORES], path[TAMANHO_PATH];
    int ano;
    bool removido;

} Metadados;



//* Funções de Inicialização

Metadados* criaMetadados(Comando* comando)
{
    if(getTipoComando(comando) != ADICIONAR)
        return NULL;

    Metadados* metadados = malloc(sizeof(Metadados));
    if(getDadosComandoAdicionar(comando, metadados->nome, metadados->autor, metadados->path, &(metadados->ano), &(metadados->removido)) == 0)
        return metadados; 
    
    free(metadados); 
    return NULL;
}



//* Free

void freeMetadados(Metadados* metadados)
{
    free(metadados);
}


void freeMetadadosVoid(void* metadados)
{
    Metadados* nMetadados = (Metadados*)metadados;
    freeMetadados(nMetadados); 
}


//* Escrever e ler no ficheiro

Metadados* readMetadados(int fd)
{
    Metadados* nMetadados = malloc(sizeof(Metadados));
    int n;

    if((n = read(fd, nMetadados, BUFFER)) <= 0)
    {
        free(nMetadados);
        return NULL;
    }    

    return nMetadados;
}


void writeMetadados(Metadados* metadados, int fd)
{
    write(fd, metadados, BUFFER);
}



//* Getters

bool isRemovido(Metadados* metadados)
{
    return metadados->removido;
}


char* getPath(Metadados* metadados)
{
    return strdup(metadados->path);
}


char* getAuthors(Metadados* metadados)
{
    return strdup(metadados->autor);
}


char* getNome(Metadados* metadados)
{
    return strdup(metadados->nome);
}


int getAno(Metadados* metadados)
{
    return metadados->ano;
}



//* Setters

void setRemovido(Metadados* metadados)
{
    metadados->removido = true;
}