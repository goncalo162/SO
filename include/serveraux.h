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
 * Cria e inicializa uma stack para gerir índices disponíveis e uma cache com capacidade limitada.
 *
 * @param tamanho Capacidade máxima da cache.
 * @return Ponteiro para a estrutura ServerAuxiliar recém-criada.
 */
ServerAuxiliar* initServerAux(int tamanho);


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


/**
 * @brief Obtém os metadados associados a um índice na cache.
 *
 * @param aux Ponteiro para a estrutura ServerAuxiliar.
 * @param index Índice do metadado a obter.
 * @return Ponteiro para os metadados se existirem na cache, ou NULL caso contrário.
 */
Metadados* cacheGet(ServerAuxiliar* aux, int index);


/**
 * @brief Insere ou atualiza um metadado na cache.
 *
 * Adiciona os metadados à cache associados ao índice dado. Se o índice já existir,
 * os metadados são atualizados. Caso a cache atinja a sua capacidade máxima, aplica-se
 * a política de substituição definida (ex. LRU).
 *
 * @param aux Ponteiro para a estrutura ServerAuxiliar.
 * @param index Índice associado aos metadados.
 * @param metadados Ponteiro para a estrutura Metadados a armazenar na cache.
 */
void cachePut(ServerAuxiliar* aux, int index, Metadados* metadados);


/**
 * @brief Remove os metadados associados a um índice da cache.
 *
 * Elimina da cache os metadados correspondentes ao índice fornecido.
 * Liberta a memória associada a esses metadados, se aplicável.
 *
 * @param aux Ponteiro para a estrutura ServerAuxiliar.
 * @param index Índice cujos metadados devem ser removidos da cache.
 */
void cacheRemove(ServerAuxiliar* aux, int index);

#endif