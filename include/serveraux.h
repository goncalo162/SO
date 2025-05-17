#ifndef SERVERAUX
#define SERVERAUX

#include "metadados.h"

/**
 * @file serveraux.h
 * @brief Declaração da estrutura auxiliar do servidor e operações associadas a uma stack de índices.
 */

/**
 * @brief Estrutura opaca que representa a estrutura auxiliar do servidor.
 *
 * A definição interna está escondida do utilizador para garantir encapsulamento.
 */
typedef struct serverAux ServerAuxiliar;

/**
 * @brief Inicializa a estrutura auxiliar do servidor.
 *
 * Cria e inicializa uma stack para gerir índices disponíveis.
 *
 * @return Ponteiro para a estrutura ServerAuxiliar recém-criada.
 */
ServerAuxiliar* initServerAux(int capacidadeCache);

/**
 * @brief Remove e devolve o índice no topo da stack.
 *
 * Esta operação remove o elemento mais recentemente inserido na stack.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar.
 * @return Índice inteiro no topo da stack, ou valor indefinido se a stack estiver vazia.
 */
int popStack(ServerAuxiliar* stack);

/**
 * @brief Insere um índice na stack.
 *
 * Adiciona um novo índice ao topo da stack.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar.
 * @param index Índice a ser inserido.
 */
void pushStack(ServerAuxiliar* stack, int index);

/**
 * @brief Verifica se a stack está vazia.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar.
 * @return true se a stack estiver vazia, false caso contrário.
 */
bool isStackEmpty(ServerAuxiliar* stack);

/**
 * @brief Liberta toda a memória associada à estrutura auxiliar.
 *
 * Deve ser chamada no final da utilização da stack para evitar fugas de memória.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar a ser libertada.
 */
void freeServerAux(ServerAuxiliar* stack);

/**
 * @brief Obtém o índice no topo da stack sem o remover.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar.
 * @return Índice inteiro no topo da stack, ou valor indefinido se a stack estiver vazia.
 */
int getIndexCabeca(ServerAuxiliar* stack);

/**
 * @brief Imprime o conteúdo atual da stack (para depuração).
 *
 * Apenas utilizada para fins de debug. Imprime os índices atualmente presentes na stack.
 *
 * @param stack Ponteiro para a estrutura ServerAuxiliar.
 */
void debugPrintStack(ServerAuxiliar *stack);

Metadados* cache_get(ServerAuxiliar* aux, int index);
void cache_put(ServerAuxiliar* aux, int index, Metadados* metadados);


#endif