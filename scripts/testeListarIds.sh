#!/bin/bash

VERDE="\e[32m"
VERMELHO="\e[31m"
AZUL="\e[34m"
AMARELO="\e[33m"
NEUTRO="\e[0m"


if [ "$#" -ne 4 ]; then
    echo -e "${VERMELHO}Uso: $0 <pasta_dataset> <ficheiro_Gcatalog> <palavra_chave> <número_de_iterações>${NEUTRO}"
    exit 1
fi

PASTA_DATASET="$1"
GCATALOG="$2"
PALAVRA_CHAVE="$3"
ITERACOES="$4"

if [ ! -d "$PASTA_DATASET" ]; then
    echo -e "${VERMELHO}Erro: Pasta '$PASTA_DATASET' não encontrada.${NEUTRO}"
    exit 1
fi

if [ ! -f "$GCATALOG" ]; then
    echo -e "${VERMELHO}Erro: Ficheiro '$GCATALOG' não encontrado.${NEUTRO}"
    exit 1
fi


VALORES_S=(1 2 4 8 16 32 64 128 256 512 800)
CACHE_SIZE=10

mkdir -p results
rm -f results/s_*.txt


calcular_estatisticas() {
    local ficheiro=$1
    local melhor=$(sort -g "$ficheiro" | head -n 1)
    local pior=$(sort -g "$ficheiro" | tail -n 1)
    local media=$(awk '{soma += $1} END {printf "%.3f", soma/NR}' "$ficheiro")
    echo "$melhor $pior $media"
}


make clean > /dev/null
make all > /dev/null


./bin/dserver "$PASTA_DATASET" "$CACHE_SIZE" &
PID_SERVIDOR=$!
sleep 1

echo -e "${AZUL}Adicionando metadados via $GCATALOG...${NEUTRO}"
./scripts/addGdatasetMetadata.sh "$GCATALOG"


echo -e "\n${AMARELO}====== Testes de Paralelismo (-s \"$PALAVRA_CHAVE\") ======${NEUTRO}"
for S in "${VALORES_S[@]}"; do
    echo -e "${AZUL}Testando com $S processos...${NEUTRO}"
    for ((i=1; i<=ITERACOES; i++)); do
        INICIO=$(date +%s%N)
        ./bin/dclient -s "$PALAVRA_CHAVE" "$S" > /dev/null
        FIM=$(date +%s%N)
        TEMPO=$(echo "scale=3; ($FIM - $INICIO)/1000000" | bc)
        echo "$TEMPO" >> "results/s_${S}.txt"
    done
done

./bin/dclient -f > /dev/null
wait $PID_SERVIDOR


echo
echo -e "${AMARELO}========== RESULTADOS -s ==========${NEUTRO}"
melhor_global=999999
s_melhor=""


for S in "${VALORES_S[@]}"; do
    FICHEIRO="results/s_${S}.txt"
    read melhor pior media < <(calcular_estatisticas "$FICHEIRO")
    echo -e "${VERDE}-s $S => Melhor: ${melhor}ms | Pior: ${pior}ms | Média: ${media}ms${NEUTRO}"

    if (( $(echo "$media < $melhor_global" | bc -l) )); then
        melhor_global=$media
        s_melhor=$S
    fi
done


echo
echo -e "${AMARELO}========== MELHOR CONFIGURAÇÃO -s ==========${NEUTRO}"
echo -e "${VERDE}✅ -s $s_melhor teve o melhor tempo médio: ${melhor_global}ms${NEUTRO}"