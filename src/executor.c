#include "executor.h"

#define RESPOSTA_TAM_MAX 200000 
#define ERRO_ABRIR_FICHEIRO "ERRO! Não foi possível abrir um ficheiro"
#define ERRO_COPIAR_DADOS_COMANDO "ERRO! Não foi possível copiar os dados do comando"
#define ERRO_COPIAR_INDEX_COMANDO "ERRO! Não foi possível copiar o index do comando"
#define ERRO_COPIAR_NUM_PROCESSOS_COMANDO "ERRO! Não foi possível copiar o número de processos do comando"
#define ERRO_COPIAR_PALAVRA_CHAVE_COMANDO "ERRO! Não foi possível copiar a palavra chave do comando"
#define ERRO_FICHEIRO_INEXISTENTE "ERRO! Esse ficheiro não existe na diretoria de ficheiros"
#define ERRO_INDEX_INEXISTENTE "ERRO! Esse index não existe no dataset ou foi removido"
#define ERRO_LER_INDEXMAX "ERRO! Não foi possível ler o index máximo do ficheiro"
#define ERRO_PROCESSO_FILHO_FALHOU "ERRO! Processo filho falhou"
#define DEVOLVE_MENSAGEM_ERRO(texto) criaMensagem(-1, true, texto) 



//* Funções Genéricas

Mensagem* limparRecursosComando(int fd, Metadados* metadados, char* palavraChave, char* mensagem, bool erro, int index) 
{
    if(fd != -1) close(fd);
    if(metadados) freeMetadados(metadados);
    if(palavraChave) free(palavraChave);
    if(erro)
        return DEVOLVE_MENSAGEM_ERRO(mensagem);
    
    return criaMensagem(index, false, mensagem);
}

char* limparRecursosString(int fd, Metadados* metadados, char* palavraChave, char* mensagem) 
{
    if(fd != -1) close(fd);
    if(metadados) freeMetadados(metadados);
    if(palavraChave) free(palavraChave);
    
    return mensagem;
}


char* construirCaminhoCompleto(Metadados* metadados, const char* ficheirosDir) 
{
    char* path = getPath(metadados);
    if(!path) return NULL;

    char* caminhoCompleto = malloc(sizeof(char) * TAMANHO_PATH * 2);
    if(!caminhoCompleto) 
    {
        free(path);
        free(caminhoCompleto);
        return NULL;
    }

    snprintf(caminhoCompleto, TAMANHO_PATH * 2, "%s/%s", ficheirosDir, path);
    free(path);
    return caminhoCompleto;
}


Mensagem* juntaRespostas(int numProcessos, int pipes[][2], int pids[])
{
    char respostaFinal[RESPOSTA_TAM_MAX] = "[";
    int primeiro = 1;
    
    for (int i = 0; i < numProcessos; i++) 
    {
        char buffer[RESPOSTA_TAM_MAX] = {0};
        read(pipes[i][0], buffer, RESPOSTA_TAM_MAX);
        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0);
    
        int len = strlen(buffer);
        if (len >= 2) 
        {
            buffer[len - 1] = '\0';
            char* resto = buffer + 1;
    
            if(strlen(resto) == 0) 
                continue;
    
            if(!primeiro) 
                strcat(respostaFinal, ", ");
            strcat(respostaFinal, resto);
            primeiro = 0;
        }
    }
    
    strcat(respostaFinal, "]");    
    return criaMensagem(-1, false, respostaFinal);   
}


Metadados* getMetadados(ServerAuxiliar* serveraux, int index, int fd)
{
    Metadados* metadados = NULL;

    metadados = cacheGet(serveraux, index);
    if(metadados == NULL)
    {
        lseek(fd, (index * BUFFER) + 4, SEEK_SET);
        if(!(metadados = readMetadados(fd)) || isRemovido(metadados))
            return NULL;
    }

    return metadados;    
}


//* Executar programas externos

bool contemPalavra(const char* palavra, const char* caminho) 
{
    int fildes[2];
    pipe(fildes);

    pid_t pid = fork();
    if (pid == 0) 
    {
        dup2(fildes[1], STDOUT_FILENO);
        close(fildes[0]);
        close(fildes[1]);
        execlp("grep", "grep", palavra, caminho, NULL);
        _exit(EXIT_FAILURE); 
    }

    close(fildes[1]);
    char buffer[BUFFER];
    int n = read(fildes[0], buffer, BUFFER);
    close(fildes[0]);

    if(n>0)
        return true;

    return false;
}


int executaGrepWc(const char* palavra, const char* caminho, char* resposta) 
{
    int pipeGrep[2], pipeWc[2];
    pipe(pipeGrep);
    pipe(pipeWc);

    pid_t pid1 = fork();
    if (pid1 == 0) 
    {
        dup2(pipeGrep[1], STDOUT_FILENO);
        close(pipeGrep[0]); close(pipeGrep[1]);
        close(pipeWc[0]); close(pipeWc[1]);
        execlp("grep", "grep", palavra, caminho, NULL);
        _exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) 
    {
        dup2(pipeGrep[0], STDIN_FILENO);
        dup2(pipeWc[1], STDOUT_FILENO);
        close(pipeGrep[0]); close(pipeGrep[1]);
        close(pipeWc[0]); close(pipeWc[1]);
        execlp("wc", "wc", "-l", NULL);
        _exit(EXIT_FAILURE);
    }

    close(pipeGrep[0]); close(pipeGrep[1]);
    close(pipeWc[1]);

    int estado;
    waitpid(pid1, &estado, 0);
    if(estado != 0) return -1;
    waitpid(pid2, &estado, 0);
    if(estado != 0) return -1;

    int n = read(pipeWc[0], resposta, RESPOSTA_TAM_MAX - 1);
    close(pipeWc[0]);
    if(n <= 0) return -1;

    resposta[n - 1] = '\0';
    return 0;
}



//* Executa Comando Adicionar

Mensagem* executaComandoAdicionar(Comando* comando, char* caminhoMetadados, char* ficheirosDir, ServerAuxiliar* serveraux)
{
    int fd = -1, index = 0;
    Metadados* metadados = NULL;
    char* caminhoCompleto = NULL;

    if((fd = open(caminhoMetadados, O_RDWR | O_CREAT, 0666)) == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_ABRIR_FICHEIRO);
    if(!(metadados = criaMetadados(comando))) return limparRecursosComando(fd, NULL, NULL, ERRO_COPIAR_DADOS_COMANDO, true, -1);
    
    if(!(caminhoCompleto = construirCaminhoCompleto(metadados, ficheirosDir))) 
    {
        free(caminhoCompleto);
        return limparRecursosComando(fd, metadados, NULL, ERRO_FICHEIRO_INEXISTENTE, true, -1);
    }

    if(isStackEmpty(serveraux))
    {
        if(ficheiroVazio(caminhoMetadados)) 
        {
            write(fd, &index, sizeof(int));
    
        }else{
            read(fd, &index, sizeof(int));
            index++;
            lseek(fd, 0, SEEK_SET);
            write(fd, &index, sizeof(int));
        }
        lseek(fd, 0, SEEK_END);

    }else{
        index = getIndexCabeca(serveraux);
        lseek(fd, (index * BUFFER) + 4, SEEK_SET);
    }

    cachePut(serveraux, index, metadados);
    writeMetadados(metadados, fd);
    char resposta[RESPOSTA_TAM_MAX];
    snprintf(resposta, RESPOSTA_TAM_MAX, "Document %d indexed", index);

    free(caminhoCompleto);
    return limparRecursosComando(fd, metadados, NULL, resposta, false, -1);
}



//* Executa Comando Consultar

Mensagem* executaComandoConsultar(Comando* comando, char* caminhoMetadados, ServerAuxiliar* serveraux)
{
    int fd = -1;
    Metadados* metadados = NULL;

    if((fd = open(caminhoMetadados, O_RDONLY)) == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_ABRIR_FICHEIRO);

    int index = getIndexComando(comando);
    if(index == -1) return limparRecursosComando(fd, NULL, NULL, ERRO_COPIAR_INDEX_COMANDO, true, -1);


    if(!(metadados = getMetadados(serveraux, index, fd))) return limparRecursosComando(fd, metadados, NULL, ERRO_INDEX_INEXISTENTE, true, -1);

    char* nome = getNome(metadados);
    char* autor = getAuthors(metadados);
    char* path = getPath(metadados);
    int ano = getAno(metadados);

    char resposta[RESPOSTA_TAM_MAX];
    snprintf(resposta, RESPOSTA_TAM_MAX, "Title: %s\nAuthors: %s\nYear: %d\nPath: %s", nome, autor, ano, path);

    free(nome);
    free(autor);
    free(path);
    return limparRecursosComando(fd, metadados, NULL, resposta, false, -1);
}



//* Executa Comando Remover

Mensagem* executaComandoRemover(Comando* comando, char* caminhoMetadados, ServerAuxiliar* serveraux)
{
    int fd = -1;
    Metadados* metadados = NULL;

    if((fd = open(caminhoMetadados, O_RDWR)) == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_ABRIR_FICHEIRO);

    int indexMax = 0, index = getIndexComando(comando);
    if(index == -1) return limparRecursosComando(fd, NULL, NULL, ERRO_COPIAR_INDEX_COMANDO, true, -1);

    read(fd, &indexMax, sizeof(int));
    if(indexMax == -1 || index > indexMax) return limparRecursosComando(fd, NULL, NULL, ERRO_INDEX_INEXISTENTE, true, -1);

    lseek(fd, (index * BUFFER) + 4, SEEK_SET);
    if(!(metadados = readMetadados(fd)) || isRemovido(metadados)) return limparRecursosComando(fd, metadados, NULL, ERRO_INDEX_INEXISTENTE, true, -1);

    cacheRemove(serveraux, index);
    setRemovido(metadados);
    lseek(fd, -BUFFER, SEEK_CUR);
    writeMetadados(metadados, fd);

    char resposta[RESPOSTA_TAM_MAX];
    snprintf(resposta, RESPOSTA_TAM_MAX, "Index entry %d deleted", index);

    return limparRecursosComando(fd, metadados, NULL, resposta, false, index);
}



//* Executa Comando Pesquisa Número de Linhas

Mensagem* executaComandoPesquisaNumLinhas(Comando* comando, char* caminhoMetadados, char* ficheirosDir, ServerAuxiliar* serveraux) 
{
    int fd = -1, index;
    char* palavraChave = NULL;
    Metadados* metadados = NULL;
    char resposta[RESPOSTA_TAM_MAX];

    if((index = getIndexComando(comando)) == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_COPIAR_INDEX_COMANDO);

    if((palavraChave = getPalavraChaveComando(comando)) == NULL) return DEVOLVE_MENSAGEM_ERRO(ERRO_COPIAR_PALAVRA_CHAVE_COMANDO);

    if((fd = open(caminhoMetadados, O_RDWR)) == -1) return limparRecursosComando(fd, NULL, palavraChave, ERRO_ABRIR_FICHEIRO, true, -1);
    if(!(metadados = getMetadados(serveraux, index, fd))) return limparRecursosComando(fd, metadados, palavraChave, ERRO_INDEX_INEXISTENTE, true, -1);

    char* caminhoCompleto;
    if(!(caminhoCompleto = construirCaminhoCompleto(metadados, ficheirosDir))) return limparRecursosComando(fd, metadados, palavraChave, ERRO_FICHEIRO_INEXISTENTE, true, -1);

    if(!ficheiroExiste(caminhoCompleto)) return limparRecursosComando(fd, metadados, palavraChave, ERRO_FICHEIRO_INEXISTENTE, true, -1);
    if(executaGrepWc(palavraChave, caminhoCompleto, resposta) != 0) return limparRecursosComando(fd, metadados, palavraChave, ERRO_PROCESSO_FILHO_FALHOU, true, -1);

    return limparRecursosComando(fd, metadados, palavraChave, resposta, false, -1);
}



//* Executa Comando Pesquisa Ids

char* executaComandoPesquisaIds(Comando* comando, char* caminhoMetadados, char* ficheirosDir, int inicio, int fim, ServerAuxiliar* serveraux)
{
    int fd = -1, indexMax = 0, offset = 1;
    char* palavraChave = NULL;
    char resposta[RESPOSTA_TAM_MAX] = "[";

    if(!(palavraChave = getPalavraChaveComando(comando))) return strdup(ERRO_COPIAR_PALAVRA_CHAVE_COMANDO);
    if((fd = open(caminhoMetadados, O_RDWR)) == -1) return limparRecursosString(fd, NULL, palavraChave, ERRO_ABRIR_FICHEIRO);

    if(fim == -1) 
    {
        if(read(fd, &indexMax, sizeof(int)) < 0) 
            return limparRecursosString(fd, NULL, palavraChave, ERRO_COPIAR_INDEX_COMANDO);

    }else{
        indexMax = fim;
    }

    for(int i = inicio; i <= indexMax; i++) 
    {
        Metadados* metadados = getMetadados(serveraux, i, fd);
        if(!metadados) 
            continue;

        char* caminhoCompleto = construirCaminhoCompleto(metadados, ficheirosDir);
        if(!caminhoCompleto) 
        {
            freeMetadados(metadados);
            return limparRecursosString(fd, NULL, palavraChave, ERRO_FICHEIRO_INEXISTENTE);
        }

        if(ficheiroExiste(caminhoCompleto) && contemPalavra(palavraChave, caminhoCompleto))
            offset += snprintf(resposta + offset, RESPOSTA_TAM_MAX - offset, "%d, ", i);

        free(caminhoCompleto);
        freeMetadados(metadados);
    }

    if(offset > 1) 
    {
        resposta[offset - 2] = ']';
        resposta[offset - 1] = '\0';
    }else{
        strcat(resposta, "]");
    }

    close(fd);
    free(palavraChave);
    return strdup(resposta);
}



//* Executa Comando Pesquisa Ids com Vários Processos

Mensagem* executaComandoPesquisaIdsMultiproc(Comando* comando, char* caminhoMetadados, char* ficheirosDir, ServerAuxiliar* serveraux)
{
    int fd, n, indexMax = 0, numProcessos = getNumProcessos(comando);  

    if (numProcessos == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_COPIAR_NUM_PROCESSOS_COMANDO);
    if((fd = open(caminhoMetadados, O_RDONLY)) == -1) return DEVOLVE_MENSAGEM_ERRO(ERRO_ABRIR_FICHEIRO);

    if((n = read(fd, &indexMax, sizeof(int))) < 0) 
    {
        close(fd);
        return DEVOLVE_MENSAGEM_ERRO(ERRO_COPIAR_INDEX_COMANDO);
    }
    close(fd);

    if(numProcessos>indexMax) numProcessos = indexMax;
    int segmento = (indexMax + 1) / numProcessos;
    int resto = (indexMax + 1) % numProcessos;

    int pipes[numProcessos][2];
    pid_t pids[numProcessos];

    for (int i = 0; i < numProcessos; i++) 
    {
        pipe(pipes[i]);

        pid_t pid = fork();
        if (pid == 0) 
        {
            close(pipes[i][0]);

            int inicio = i * segmento, fim = 0;
            {
                if(i < resto)
                {
                    inicio += i;
                    fim = inicio + segmento - 1;
                }else{
                    inicio += resto;
                    fim += inicio + segmento - 2;
                }
            }

            Comando* subcomando = NULL;
            if(numProcessos == 1)
            {
                fim = -1;
                subcomando = comando;
            }else{
                subcomando = comandoMultiprocParaId(comando);
            }

            char* resposta = executaComandoPesquisaIds(subcomando, caminhoMetadados, ficheirosDir, inicio, fim, serveraux);
            write(pipes[i][1], resposta, strlen(resposta) + 1);

            freeComando(subcomando);
            free(resposta);
            close(pipes[i][1]);
            _exit(0);
        }

        pids[i] = pid;
        close(pipes[i][1]);
    }

    return juntaRespostas(numProcessos, pipes, pids);
}



//* Funções de Execução dos Comandos

Mensagem* executaComando(Comando* comando, char* caminhoMetadados, char* ficheirosDir, ServerAuxiliar* serveraux)
{
    Mensagem* resultado = NULL;
    int tipo = getTipoComando(comando);
    switch(tipo)
    {
        case ADICIONAR: resultado = executaComandoAdicionar(comando, caminhoMetadados, ficheirosDir, serveraux); break; 
        case CONSULTAR: resultado = executaComandoConsultar(comando, caminhoMetadados, serveraux); break;
        case REMOVER: resultado = executaComandoRemover(comando, caminhoMetadados, serveraux); break;
        case PESQUISA_NUM_LINHAS: resultado = executaComandoPesquisaNumLinhas(comando, caminhoMetadados, ficheirosDir, serveraux); break;
        case PESQUISA_IDS: resultado = executaComandoPesquisaIdsMultiproc(comando, caminhoMetadados, ficheirosDir, serveraux); break;
        case PESQUISA_IDS_MULTIPROC: resultado = executaComandoPesquisaIdsMultiproc(comando, caminhoMetadados, ficheirosDir, serveraux); break;
        case FECHAR: resultado = NULL; break;
        default: resultado = NULL; break;
    }
    free(ficheirosDir);
    return resultado;
}