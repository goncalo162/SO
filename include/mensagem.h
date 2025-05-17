#ifndef MENSAGEM
#define MENSAGEM

#define TAMANHO_PACOTE 4000

/**
 * @file mensagem.h
 * @brief Definições e funções para a manipulação de mensagens trocadas entre cliente e servidor.
 */
#include "utils.h"

/**
 * @def TAMANHO_PACOTE
 * @brief Tamanho máximo de um pacote de mensagem (em bytes).
 */
#define TAMANHO_PACOTE 4000

/**
 * @brief Estrutura opaca que representa uma mensagem.
 */
typedef struct mensagem Mensagem;

/**
 * @brief Cria uma nova mensagem.
 *
 * @param valorIndex Índice associado à mensagem (ex: ID do documento).
 * @param isErro Indica se a mensagem representa um erro.
 * @param info Informação adicional a incluir na mensagem.
 * @return Ponteiro para a nova estrutura Mensagem.
 */
Mensagem* criaMensagem(int valorIndex, bool isErro, char* info);

/**
 * @brief Liberta a memória ocupada por uma mensagem.
 *
 * @param mensagem Ponteiro para a estrutura Mensagem a ser libertada.
 */
void freeMensagem(Mensagem* mensagem);

/**
 * @brief Escreve a mensagem para um descritor de ficheiro.
 *
 * @param mensagem Ponteiro para a estrutura Mensagem.
 * @param fd Descritor de ficheiro onde a mensagem será escrita.
 */
void writeMensagem(Mensagem* mensagem, int fd);

/**
 * @brief Lê uma mensagem de um descritor de ficheiro como string.
 *
 * @param fd Descritor de ficheiro de onde ler a mensagem.
 * @return String lida (deve ser libertada pelo utilizador).
 */
char* readMensagem(int fd);

/**
 * @brief Escreve a mensagem de erro para stderr.
 *
 * @param mensagem Ponteiro para a estrutura Mensagem.
 */
void perrorMensagem(Mensagem* mensagem);

/**
 * @brief Obtém o índice associado à mensagem.
 *
 * @param mensagem Ponteiro para a estrutura Mensagem.
 * @return Índice inteiro associado.
 */
int getIndexRelevante(Mensagem* mensagem);

/**
 * @brief Verifica se a mensagem representa um erro.
 *
 * @param mensagem Ponteiro para a estrutura Mensagem.
 * @return true se for erro, false caso contrário.
 */
bool isMensagemErro(Mensagem* mensagem);


#endif