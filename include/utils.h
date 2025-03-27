#ifndef UTILS
#define UTILS

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>


// Recebe uma string e retorna true caso todos os seus carateres sejam algarismos
bool isNumero(char* texto);


#endif