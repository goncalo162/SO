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


// Recebe uma string e retorna true caso todos os seus carateres sejam algarismos
bool isNumero(char* texto);

//Recebe um caminho para um ficheiro e diz se este existe ou não
bool ficheiroExiste(const char* caminho);

//Recebe um caminho para um ficheiro e diz se este está vazio (tamanho de 0 bytes) ou não
bool ficheiroVazio(const char* caminho);

#endif