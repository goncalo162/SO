#include "serveraux.h"

#define FICHEIRO_STACK_NOME "stackState"



//* Typedef

typedef struct serverAux
{
    GArray* indicesLivres;
    GHashTable* cache;
    GQueue* ordemCache;
    
} ServerAuxiliar;



//* Push e Pop da Stack

void pushStack(ServerAuxiliar* stack, int index) 
{
    g_array_append_val(stack->indicesLivres, index);
}


int popStack(ServerAuxiliar* stack)
{
    if (!isStackEmpty(stack)) 
    {
        int index = g_array_index(stack->indicesLivres, int, stack->indicesLivres->len - 1);
        g_array_remove_index(stack->indicesLivres, stack->indicesLivres->len - 1);
        return index;
    }
    return -1; 
}



//* Getters da Stack

bool isStackEmpty(ServerAuxiliar* stack) 
{
    return stack->indicesLivres->len == 0;
}


int getIndexCabeca(ServerAuxiliar* stack)
{
    return g_array_index(stack->indicesLivres, int, stack->indicesLivres->len - 1);
}


void debugPrintStack(ServerAuxiliar *stack)
{
    printf("STACK (top to bottom):\n");
    if (isStackEmpty(stack)) {
        printf("  [empty]\n");
        return;
    }

    for (int i = stack->indicesLivres->len - 1; i >= 0; i--) {
        int index = g_array_index(stack->indicesLivres, int, i);
        printf("  %d\n", index);
    }
    printf("\n");
}



//* Read e Write

void writeStack(ServerAuxiliar* stack)
{
    int fd;
    if((fd = open(FICHEIRO_STACK_NOME, O_CREAT|O_TRUNC|O_WRONLY, 0666)) == -1) return;

    int tamanho = stack->indicesLivres->len;
    write(fd, &tamanho, sizeof(int));

    for(int i=0; i<tamanho; i++)
    {
        int index = g_array_index(stack->indicesLivres, int, i);
        write(fd, &index, sizeof(int));
    }
}


void readStack(ServerAuxiliar* stack)
{
    int fd;
    stack->indicesLivres = g_array_new(FALSE, FALSE, sizeof(int)); 
    if((fd = open(FICHEIRO_STACK_NOME, O_RDONLY)) == -1)
        return;

    int tamanho;
    read(fd, &tamanho, sizeof(int));
    
    for(int i=0; i<tamanho; i++)
    {
        int indexAtual;
        lseek(fd, 0, SEEK_CUR);
        read(fd, &indexAtual, sizeof(int));
        g_array_append_val(stack->indicesLivres, indexAtual);
    }
}



//* Funções de Inicialização

ServerAuxiliar* initServerAux() 
{
    ServerAuxiliar* nServerAux = malloc(sizeof(ServerAuxiliar));

    readStack(nServerAux);
    nServerAux->cache = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, freeMetadadosVoid);
    nServerAux->ordemCache = g_queue_new();

    return nServerAux;
}



//* Frees

void freeServerAux(ServerAuxiliar* stack) 
{
    writeStack(stack);

    g_array_free(stack->indicesLivres, TRUE);
    g_hash_table_destroy(stack->cache);
    g_queue_free(stack->ordemCache);
}