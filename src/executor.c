#include "executor.h"

#define RESPOSTA_TAM_MAX 560 //Número máximo de bytes que uma resposta pode ter
#define ERRO_ABRIR_FICHEIRO "\\ERRO! Não foi possível abrir um ficheiro\n\0"
#define ERRO_COPIAR_DADOS_COMANDO "\\ERRO! Não foi possível copiar os dados do comando\n\0"
#define ERRO_COPIAR_INDEX_COMANDO "\\ERRO! Não foi possível copiar o index do comando\n\0"
#define ERRO_COPIAR_PALAVRA_CHAVE_COMANDO "\\ERRO! Não foi possível copiar a palavra chave do comando\n\0"
#define ERRO_FICHEIRO_INEXISTENTE "\\ERRO! Esse ficheiro não existe na diretoria de ficheiros\n\0"
#define ERRO_INDEX_INEXISTENTE "\\ERRO! Esse index não existe no dataset ou foi removido\n\0"
#define ERRO_PROCESSO_FILHO_FALHOU "\\ERRO! Processo filho falhou\n\0"



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
        freeMetadados(metadados);
        close(fd);
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
    if((metadados = readMetadados(fd)) == NULL) goto errosIndex;

    if(isRemovido(metadados) == true)
    {
        goto errosIndexComMetadados;
    
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

    errosIndexComMetadados: freeMetadados(metadados);
    errosIndex:             close(fd);
                            return strdup(ERRO_INDEX_INEXISTENTE);
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
    if(index == -1 || index < indexComando) goto errosIndex;   

    Metadados* metadados;
    lseek(fd, (indexComando * BUFFER) + 4, SEEK_SET);
    
    if(!(metadados = readMetadados(fd))) goto errosIndex;

    if(isRemovido(metadados) == true)
    {
        goto errosIndexComMetadados;

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

    errosIndexComMetadados: freeMetadados(metadados);
    errosIndex:             close(fd);
                            return strdup(ERRO_INDEX_INEXISTENTE);
}


char* executaComandoPesquisaNumLinhas(Comando* comando, char* caminhoMetadados, char* ficheirosDir)
{
    int fd, index;
    int pipe_grep[2], pipe_wc[2];
    char resposta[RESPOSTA_TAM_MAX];
    char* palavaraChave;

    if ((index = getIndexComando(comando)) == -1) return strdup(ERRO_COPIAR_INDEX_COMANDO);
    if ((palavaraChave = getPalavraChaveComando(comando)) == NULL) return strdup(ERRO_COPIAR_PALAVRA_CHAVE_COMANDO);
    if ((fd = open(caminhoMetadados, O_RDWR)) == -1) return strdup(ERRO_ABRIR_FICHEIRO);

    lseek(fd, (index * BUFFER) + 4, SEEK_SET);
    Metadados* metadados;
    if (!(metadados = readMetadados(fd))) goto errosIndex;
    if (isRemovido(metadados) == true) goto errosIndexComMetadados;

    char* path = getPath(metadados);
    char caminhoCompleto[TAMANHO_PATH * 2];
    snprintf(caminhoCompleto, TAMANHO_PATH * 2, "%s/%s", ficheirosDir, path);
    free(path);

    if (!ficheiroExiste(caminhoCompleto)) 
    {
        freeMetadados(metadados);
        free(palavaraChave);
        close(fd);
        return strdup(ERRO_FICHEIRO_INEXISTENTE);
    }

    pipe(pipe_grep); // grep -> wc
    pipe(pipe_wc);   // wc -> pai

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipe_grep[1], STDOUT_FILENO);
        close(pipe_grep[0]);
        close(pipe_grep[1]);
        close(pipe_wc[0]);
        close(pipe_wc[1]);

        execlp("grep", "grep", palavaraChave, caminhoCompleto, NULL);
        _exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) 
    {
        dup2(pipe_grep[0], STDIN_FILENO);
        dup2(pipe_wc[1], STDOUT_FILENO);

        close(pipe_grep[0]);
        close(pipe_grep[1]);
        close(pipe_wc[0]);
        close(pipe_wc[1]);

        execlp("wc", "wc", "-l", NULL);
        _exit(EXIT_FAILURE);
    }

    close(pipe_grep[0]);
    close(pipe_grep[1]);
    close(pipe_wc[1]);

    int estado;
    waitpid(pid1, &estado, 0);
    if (estado != 0) goto errosProcessoFilho;

    waitpid(pid2, &estado, 0);
    if (estado != 0) goto errosProcessoFilho;

    int n;
    if ((n = read(pipe_wc[0], resposta, RESPOSTA_TAM_MAX - 1)) <= 0) goto errosProcessoFilho;
    resposta[strlen(resposta)-1] = '\0';

    close(pipe_wc[0]);
    close(fd);
    freeMetadados(metadados);
    free(palavaraChave);
    return strdup(resposta);

    errosIndexComMetadados: freeMetadados(metadados);
    errosIndex:             free(palavaraChave);
                            close(fd);
                            return strdup(ERRO_INDEX_INEXISTENTE);

    errosProcessoFilho:     close(pipe_wc[0]);
                            close(fd);
                            freeMetadados(metadados);
                            free(palavaraChave);
                            return strdup(ERRO_PROCESSO_FILHO_FALHOU);
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
        case PESQUISA_NUM_LINHAS: resultado = executaComandoPesquisaNumLinhas(comando, caminhoMetadados, ficheirosDir); break;
        case FECHAR: resultado = NULL; break;
        default: resultado = " "; break;
    }
    free(ficheirosDir);
    return resultado;
}