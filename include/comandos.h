#ifndef COMANDOS
#define COMANDOS

#include "utils.h"

#define TAMANHO_AUTORES 200 //Tamanho máximo em bytes para os autores
#define TAMANHO_TITULO 200 //Tamanho máximo em bytes para o título
#define TAMANHO_PATH 64 //Tamanho máximo em bytes para a path
#define BUFFER 512 //Tamanho máximo em bytes que o comando adicionar pode ter

/* Estrutura de dados que representa um comando e os seus argumentos
   Contém:
   Um tipo comando
   Os argumentos passados ao comando
*/
typedef struct comando Comando;

//Cria um comando se este for válido com os argumentos passados a este e o número de argumentos
Comando* criaComando(char* argumentos[], int tamanho);

//Cria um comando vazio (útil para receber dados de outros comandos)
Comando* criaComandoVazio();

//Liberta o espaço em memória ocupado por por um comando
void freeComando(Comando* comando);

int writeComando(int fd, Comando* comando);

Comando* readComando(int fd);

int getDadosComandoAdicionar(Comando* comando, char nomeCopia[TAMANHO_TITULO], char autoresCopia[TAMANHO_AUTORES], char pathCopia[TAMANHO_PATH], int* anoCopia, bool* removidoCopia);

int getTipoComando(Comando* comando);

int getIndexComando(Comando* comando);


#endif