#ifndef COMANDOS
#define COMANDOS

#include "utils.h"

#define TAMANHO_AUTORES 200 // Tamanho máximo em bytes para os autores
#define TAMANHO_TITULO 200  // Tamanho máximo em bytes para o título
#define TAMANHO_PATH 64     // Tamanho máximo em bytes para a path
#define TAMANHO_PALAVRA_CHAVE 400 // Tamanho máximo em bytes para a palavra chave

/* Enum constante com os valores para cada tipo de comando */
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

/* Estrutura de dados que representa um comando e os seus argumentos */
typedef struct comando Comando;

// Cria um comando se este for válido com os argumentos passados e o número de argumentos
Comando* criaComando(char* argumentos[], int tamanho, pid_t pid);

// Cria um novo comando PESQUISA_IDS a partir de PESQUISA_IDS_MULTIPROC (com numProc = 1)
Comando* comandoMultiprocParaId(Comando* comando);

// Liberta o espaço em memória ocupado por um comando
void freeComando(Comando* comando);

// Escreve ou lê um comando de um file descriptor
int writeComando(int fd, Comando* comando);
Comando* readComando(int fd, int* n);

// Getters para os argumentos de comandos
int getDadosComandoAdicionar(Comando* comando, char nomeCopia[TAMANHO_TITULO], char autoresCopia[TAMANHO_AUTORES], char pathCopia[TAMANHO_PATH], int* anoCopia, bool* removidoCopia);

int getTipoComando(Comando* comando);
int getIndexComando(Comando* comando);
char* getPalavraChaveComando(Comando* comando);
int getNumProcessos(Comando* comando);
pid_t getPidCliente(Comando* comando);

#endif
