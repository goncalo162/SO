#include "executor.h"

#define RESPOSTA_TAM_MAX 560 //Número máximo de bytes que uma resposta pode ter
#define ERRO_ABRIR_FICHEIRO "\\ERRO! Não foi possível abrir um ficheiro\n\0"
#define ERRO_COPIAR_DADOS_COMANDO "\\ERRO! Não foi possível copiar os dados do comando\n\0"
#define ERRO_COPIAR_INDEX_COMANDO "\\ERRO! Não foi possível copiar o index do comando\n\0"
#define ERRO_FICHEIRO_INEXISTENTE "\\ERRO! Esse ficheiro não existe na diretoria de ficheiros\n\0"
#define ERRO_INDEX_INEXISTENTE "\\ERRO! Esse index não existe no dataset ou foi removido\n\0"



//* Funções de Execução Auxiliares

char* executaComandoAdicionar(Comando* comando, char* caminhoMetadados, char* ficheirosDir)
{
    int fd; 
    if((fd = open(caminhoMetadados, O_RDWR|O_CREAT, 0666)) == -1) return strdup(ERRO_ABRIR_FICHEIRO);

    int index = 0;

    if(ficheiroVazio(caminhoMetadados)) //Se o ficheiro de metadados for vazio adicionamos ao inicio do ficheiro o primeiro index válido (0)
    {
        lseek(fd, 0, SEEK_SET);
        write(fd, &index, sizeof(int));

    }else{ //Caso contrário lemos o index no inicio do ficheiro e substituimos com o valor novo
        lseek(fd, 0, SEEK_SET);
        read(fd, &index, sizeof(int));
        index++;
        lseek(fd, 0, SEEK_SET);
        write(fd, &index, sizeof(int));
    }
    lseek(fd, 0, SEEK_END);

    Metadados* metadados;
    if(!(metadados = criaMetadados(comando)))
    {
        close(fd);
        return strdup(ERRO_COPIAR_DADOS_COMANDO);
    }
    char* path = getPath(metadados);
    char caminhoCompleto[TAMANHO_PATH*2];
    snprintf(caminhoCompleto, TAMANHO_PATH*2, "%s/%s", ficheirosDir, path);
    if(!ficheiroExiste(caminhoCompleto)) 
    {
        free(path);
        return strdup(ERRO_FICHEIRO_INEXISTENTE);
    }

    writeMetadados(metadados, fd);

    char resposta[RESPOSTA_TAM_MAX];
    snprintf(resposta, RESPOSTA_TAM_MAX,"Document %d indexed", index);

    close(fd);
    free(path);
    freeMetadados(metadados);
    return strdup(resposta);
}


char* executaComandoConsultar(Comando* comando, char* caminhoMetadados)
{
    int fd; 
    if((fd = open(caminhoMetadados, O_RDONLY)) == -1) return strdup(ERRO_ABRIR_FICHEIRO);
    
    char mensagem[BUFFER];
    int index;
    if((index = getIndexComando(comando)) == -1) return strdup(ERRO_COPIAR_INDEX_COMANDO);

    Metadados* metadados;
    lseek(fd, (index * BUFFER) + 4, SEEK_SET);
    
    if((metadados = readMetadados(fd)) == NULL)
    {
        close(fd);
        return strdup(ERRO_INDEX_INEXISTENTE);        
    }

    if(isRemovido(metadados) == true)
    {
        close(fd);
        freeMetadados(metadados);
        return strdup(ERRO_INDEX_INEXISTENTE);
    
    }else{
        close(fd);

        char* nome = getNome(metadados);
        char* autor = getAuthors(metadados);
        char* path = getPath(metadados);
        int ano = getAno(metadados);

        char resposta[RESPOSTA_TAM_MAX];
        snprintf(resposta, RESPOSTA_TAM_MAX,"Title: %s\nAuthors: %s\nYear: %d\nPath: %s", nome, autor, ano, path);    

        free(nome);
        free(path);
        free(autor);

        return strdup(resposta);
    }
}


char* executaComandoRemover(Comando* comando, char* caminhoMetadados)
{
    int fd; 
    if((fd = open(caminhoMetadados, O_RDWR)) == -1) return strdup(ERRO_ABRIR_FICHEIRO);
    
    char mensagem[BUFFER];
    int index, indexComando;

    if((indexComando = getIndexComando(comando)) == -1) 
    {
        close(fd);
        return strdup(ERRO_COPIAR_INDEX_COMANDO);
    }    

    lseek(fd, 0, SEEK_SET);
    read(fd, &index, sizeof(int));
    
    if(index == -1 || index < indexComando)
    {
        close(fd);
        return strdup(ERRO_INDEX_INEXISTENTE);
    }     

    Metadados* metadados;
    lseek(fd, (indexComando * BUFFER) + 4, SEEK_SET);
    
    if(!(metadados = readMetadados(fd)))
    {
        close(fd);
        return strdup(ERRO_INDEX_INEXISTENTE);        
    }

    if(isRemovido(metadados) == true)
    {
        close(fd);
        freeMetadados(metadados);
        return strdup(ERRO_INDEX_INEXISTENTE);

    }else{
        setRemovido(metadados);
        lseek(fd, -BUFFER, SEEK_CUR);
        writeMetadados(metadados, fd);
        freeMetadados(metadados);
        close(fd);

        char resposta[RESPOSTA_TAM_MAX]; 
        snprintf(resposta, RESPOSTA_TAM_MAX, "Index entry %d deleted", indexComando);
        return strdup(resposta);
    }
}



//* Funções de Execução dos Comandos

char* executaComando(Comando* comando, char* caminhoMetadados, char* ficheirosDir)
{
    char* resultado = NULL;
    int tipo = getTipoComando(comando);
    switch(tipo)
    {
        case ADICIONAR: resultado = executaComandoAdicionar(comando, caminhoMetadados, ficheirosDir); break; 
        case CONSULTAR: resultado = executaComandoConsultar(comando, caminhoMetadados); break;
        case REMOVER: resultado = executaComandoRemover(comando, caminhoMetadados); break;
        case FECHAR: resultado = NULL; break;
        default: resultado = " "; break;
    }
    free(ficheirosDir);
    return resultado;
}