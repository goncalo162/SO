#ifndef COMANDOS
#define COMANDOS

/**
 * @file comandos.h
 * @brief Estruturas e funções para criação, leitura e escrita de comandos do cliente.
 */

#include "utils.h"

/** Tamanho máximo em bytes para os autores */
#define TAMANHO_AUTORES 200 
/** Tamanho máximo em bytes para o título */
#define TAMANHO_TITULO 200  
/** Tamanho máximo em bytes para a path */
#define TAMANHO_PATH 64 
/** Tamanho máximo em bytes para a palavra-chave */    
#define TAMANHO_PALAVRA_CHAVE 400 

/**
 * @brief Enumeração com os tipos de comando disponíveis.
 */
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

/**
 * @brief Estrutura opaca que representa um comando.
 */
typedef struct comando Comando;

/**
 * @brief Cria um comando válido a partir dos argumentos recebidos.
 *
 * @param argumentos Vetor de strings com os argumentos.
 * @param tamanho Número de argumentos.
 * @param pid PID do cliente que emitiu o comando.
 * @return Ponteiro para o novo comando, ou NULL se inválido.
 */
Comando* criaComando(char* argumentos[], int tamanho, pid_t pid);

/**
 * @brief Converte um comando do tipo PESQUISA_IDS_MULTIPROC para PESQUISA_IDS.
 *
 * @param comando Comando original com múltiplos processos.
 * @return Novo comando com numProc = 1.
 */
Comando* comandoMultiprocParaId(Comando* comando);

/**
 * @brief Liberta a memória ocupada por um comando.
 *
 * @param comando Ponteiro para a estrutura Comando a libertar.
 */
void freeComando(Comando* comando);

/**
 * @brief Escreve um comando para um descritor de ficheiro.
 *
 * @param fd Descritor de ficheiro.
 * @param comando Comando a escrever.
 * @return Número de bytes escritos ou -1 em caso de erro.
 */
int writeComando(int fd, Comando* comando);
/**
 * @brief Lê um comando de um descritor de ficheiro.
 *
 * @param fd Descritor de ficheiro.
 * @param n Ponteiro para inteiro onde será guardado o número de argumentos lidos.
 * @return Ponteiro para a estrutura Comando lida.
 */
Comando* readComando(int fd, int* n);

/**
 * @brief Obtém os dados de um comando ADICIONAR.
 *
 * Copia os argumentos do comando para as variáveis fornecidas.
 *
 * @param comando Ponteiro para a estrutura Comando.
 * @param nomeCopia Buffer onde será copiado o nome.
 * @param autoresCopia Buffer onde serão copiados os autores.
 * @param pathCopia Buffer onde será copiado o caminho.
 * @param anoCopia Ponteiro onde será guardado o ano.
 * @param removidoCopia Ponteiro onde será guardado o estado de remoção.
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */
int getDadosComandoAdicionar(Comando* comando, char nomeCopia[TAMANHO_TITULO], char autoresCopia[TAMANHO_AUTORES], char pathCopia[TAMANHO_PATH], int* anoCopia, bool* removidoCopia);

/**
 * @brief Obtém o tipo do comando.
 */
int getTipoComando(Comando* comando);

/**
 * @brief Obtém o índice associado ao comando.
 */
int getIndexComando(Comando* comando);

/**
 * @brief Obtém a palavra-chave do comando (para pesquisa).
 */
char* getPalavraChaveComando(Comando* comando);

/**
 * @brief Obtém o número de processos (em comandos multiproc).
 */
int getNumProcessos(Comando* comando);

/**
 * @brief Obtém o PID do cliente que emitiu o comando.
 */
pid_t getPidCliente(Comando* comando);

#endif
