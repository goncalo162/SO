#ifndef EXECUTOR
#define EXECUTOR

/**
 * @file executor.h
 * @brief Função principal responsável por executar comandos recebidos pelo servidor.
 */

#include "serveraux.h"
#include "mensagem.h"

typedef struct metadados Metadados;


/**
 * @brief Executa um comando utilizando os dados do servidor.
 *
 * Esta função trata de todas as operações associadas a um comando: adicionar, consultar,
 * remover ou pesquisar metadados.
 *
 * @param comando Ponteiro para o comando a executar.
 * @param caminhoMetadados Caminho para o ficheiro de metadados.
 * @param ficheirosDir Diretório onde estão os ficheiros dos documentos.
 * @param serveraux Estrutura auxiliar do servidor (para gestão de índices).
 * @return Ponteiro para a estrutura Mensagem resultante da execução.
 */
Mensagem* executaComando(Comando* comando, char* caminhoMetadados, char* ficheirosDir, ServerAuxiliar* serveraux);


#endif