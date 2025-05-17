#ifndef UTILS
#define UTILS

#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/**
 * @file utils.h
 * @brief Declaração de funções utilitárias para manipulação de ficheiros e verificação de strings numéricas.
 */


/**
 * @brief Verifica se uma string representa um número inteiro não negativo.
 *
 * Percorre todos os caracteres da string e verifica se todos são dígitos entre '0' e '9'.
 *
 * @param texto A string a ser verificada.
 * @return true se a string for composta apenas por algarismos, false caso contrário.
 */
bool isNumero(char* texto);

/**
 * @brief Verifica se um ficheiro existe no caminho especificado.
 *
 * Utiliza `stat` para determinar se o ficheiro existe.
 *
 * @param caminho Caminho para o ficheiro a verificar.
 * @return true se o ficheiro existir, false caso contrário.
 */
bool ficheiroExiste(const char* caminho);

/**
 * @brief Verifica se um ficheiro está vazio.
 *
 * Considera um ficheiro vazio se o seu tamanho for 0 bytes. Caso não seja possível
 * aceder ao ficheiro, assume-se que está vazio (retorna true).
 *
 * @param caminho Caminho para o ficheiro a verificar.
 * @return true se o ficheiro estiver vazio ou inacessível, false caso contrário.
 */
bool ficheiroVazio(const char* caminho);

#endif