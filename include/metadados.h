#ifndef METADADOS
#define METADADOS

#include "comandos.h"

/**
 * @file metadados.h
 * @brief Declarações das funções e estrutura associadas à gestão dos metadados dos documentos.
 */

/**
 * @def BUFFER
 * @brief Tamanho máximo, em bytes, que a estrutura de metadados pode ocupar.
 */

#define BUFFER 472

/**
 * @brief Estrutura opaca que representa os metadados de um documento.
 *
 * A definição interna está escondida para garantir encapsulamento.
 */
typedef struct metadados Metadados;

/**
 * @brief Cria uma nova estrutura de metadados a partir de um comando.
 *
 * Constrói um objeto `Metadados` com base na informação contida num `Comando`.
 *
 * @param comando Ponteiro para a estrutura Comando.
 * @return Ponteiro para a estrutura Metadados criada.
 */
Metadados* criaMetadados(Comando* comando);

/**
 * @brief Liberta a memória ocupada pelos metadados.
 *
 * @param metadados Ponteiro para a estrutura Metadados a ser libertada.
 */
void freeMetadados(Metadados* metadados);

/**
 * @brief Função genérica para libertar metadados quando usados com estruturas da GLib.
 *
 * Pode ser usada como função de destruição, por exemplo, em tabelas hash.
 *
 * @param metadados Ponteiro genérico para a estrutura Metadados.
 */
void freeMetadadosVoid(void* metadados);

/**
 * @brief Verifica se os metadados estão marcados como removidos.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 * @return true se estiverem marcados como removidos, false caso contrário.
 */
bool isRemovido(Metadados* metadados);

/**
 * @brief Obtém o caminho do ficheiro a partir dos metadados.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 * @return String com o caminho do ficheiro.
 */
char* getPath(Metadados* metadados);

/**
 * @brief Obtém a lista de autores dos metadados.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 * @return String com os autores.
 */
char* getAuthors(Metadados* metadados);

/**
 * @brief Obtém o nome (título) do documento.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 * @return String com o nome do documento.
 */
char* getNome(Metadados* metadados);

/**
 * @brief Obtém o ano associado ao documento.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 * @return Ano (inteiro) do documento.
 */
int getAno(Metadados* metadados);

/**
 * @brief Marca os metadados como removidos.
 *
 * Esta função altera o estado interno dos metadados para indicar que foram removidos.
 *
 * @param metadados Ponteiro para a estrutura Metadados.
 */
void setRemovido(Metadados* metadados);

/**
 * @brief Lê uma estrutura de metadados de um descritor de ficheiro.
 *
 * @param fd Descritor de ficheiro de onde ler.
 * @return Ponteiro para a estrutura Metadados lida.
 */
Metadados* readMetadados(int fd);

/**
 * @brief Escreve os metadados para um descritor de ficheiro.
 *
 * @param metadados Ponteiro para a estrutura Metadados a ser escrita.
 * @param fd Descritor de ficheiro onde escrever.
 */
void writeMetadados(Metadados* metadados, int fd);


#endif