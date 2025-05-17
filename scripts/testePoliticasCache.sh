#!/bin/bash


if [ "$#" -ne 2 ]; then
    echo -e "${VERMELHO}Uso: $0 <pasta_dataset> <número_de_iterações>${NEUTRO}"
    exit 1
fi

PASTA_DATASET="$1"
ITERACOES="$2"


if [ ! -d "$PASTA_DATASET" ]; then
    echo -e "${VERMELHO}Erro: Pasta '$PASTA_DATASET' não encontrada.${NEUTRO}"
    exit 1
fi

TAMANHOS_CACHE=(5 10 20)
POLITICAS=("-DLRU" "-DFIFO")
ID_DOCUMENTO_CONSULTA=1
FICHEIRO_INDEXAR="1112.txt"

mkdir -p results
rm -f results/cache_*.txt

VERDE="\e[32m"
VERMELHO="\e[31m"
AZUL="\e[34m"
AMARELO="\e[33m"
NEUTRO="\e[0m"


calcular_estatisticas() {
    local ficheiro=$1
    local melhor=$(sort -g "$ficheiro" | head -n 1)
    local pior=$(sort -g "$ficheiro" | tail -n 1)
    local media=$(awk '{soma += $1} END {printf "%.3f", soma/NR}' "$ficheiro")
    echo "$melhor $pior $media"
}

for TAMANHO in "${TAMANHOS_CACHE[@]}"; do
    echo -e "${AMARELO}====== Tamanho da Cache: $TAMANHO ======${NEUTRO}"
    for POLITICA in "${POLITICAS[@]}"; do
        NOME_POLITICA="${POLITICA//-D/}"

        echo -e "${AZUL}A compilar com $NOME_POLITICA...${NEUTRO}"
        make clean > /dev/null
        make all CACHE=$POLITICA > /dev/null

        echo -e "${AZUL}A lançar o dserver com cache $TAMANHO...${NEUTRO}"
        ./bin/dserver "$PASTA_DATASET" "$TAMANHO" &
        PID_SERVIDOR=$!
        sleep 1

        echo -e "${AZUL}A indexar documento de teste...${NEUTRO}"
        ./bin/dclient -a "Romeu e Julieta" "William Shakespeare" "1997" "$FICHEIRO_INDEXAR" > /dev/null

        echo -e "${AZUL}Testar política $NOME_POLITICA por $ITERACOES iterações...${NEUTRO}"
        for ((i=1; i<=ITERACOES; i++)); do
            INICIO=$(date +%s%N)
            ./bin/dclient -c $ID_DOCUMENTO_CONSULTA > /dev/null
            FIM=$(date +%s%N)
            TEMPO_REAL=$(echo "scale=3; ($FIM - $INICIO)/1000000" | bc)  # tempo em milissegundos
            echo "$TEMPO_REAL" >> "results/cache_${NOME_POLITICA}_${TAMANHO}.txt"
        done

        ./bin/dclient -f > /dev/null
        wait $PID_SERVIDOR
    done
done

echo
echo -e "${AMARELO}========== RESUMO ==========${NEUTRO}"
declare -A vitorias

for TAMANHO in "${TAMANHOS_CACHE[@]}"; do
    echo -e "${AMARELO}--- Cache: $TAMANHO ---${NEUTRO}"
    declare -A tempos
    for POLITICA in "${POLITICAS[@]}"; do
        NOME_POLITICA="${POLITICA//-D/}"
        FICHEIRO="results/cache_${NOME_POLITICA}_${TAMANHO}.txt"
        read melhor pior media < <(calcular_estatisticas "$FICHEIRO")
        echo -e "${VERDE}${NOME_POLITICA} => Melhor: ${melhor}ms | Pior: ${pior}ms | Média: ${media}ms${NEUTRO}"
        tempos[$NOME_POLITICA]=$media
    done

    if (( $(echo "${tempos[LRU]} < ${tempos[FIFO]}" | bc -l) )); then
        vitorias[LRU]=$((vitorias[LRU]+1))
    elif (( $(echo "${tempos[FIFO]} < ${tempos[LRU]}" | bc -l) )); then
        vitorias[FIFO]=$((vitorias[FIFO]+1))
    fi
done


echo
echo -e "${AMARELO}========== VENCEDOR FINAL ==========${NEUTRO}"
echo -e "LRU venceu: ${vitorias[LRU]:-0} testes"
echo -e "FIFO venceu: ${vitorias[FIFO]:-0} testes"

if [[ ${vitorias[LRU]:-0} -gt ${vitorias[FIFO]:-0} ]]; then
    echo -e "${VERDE}LRU teve melhor desempenho no geral.${NEUTRO}"
elif [[ ${vitorias[FIFO]:-0} -gt ${vitorias[LRU]:-0} ]]; then
    echo -e "${VERDE}FIFO teve melhor desempenho no geral.${NEUTRO}"
else
    echo -e "${AMARELO}Empate${NEUTRO}"
fi