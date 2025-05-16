#!/bin/bash
# Script de teste unitário da cache para uma política (ex: LRU)

DOC_DIR="ficheiros"
CACHE_SIZE=2
POLITICA=${1:-LRU}

echo "🚀 Teste de cache com política: $POLITICA"

# Limpa ambiente
rm -f metadados stackState
mkdir -p resultados

LOG="resultados/debug_${POLITICA}.log"
rm -f "$LOG"

# Inicia servidor
./bin/dserver "$DOC_DIR" "$CACHE_SIZE" "$POLITICA" > "$LOG" 2>&1 &
SERVER_PID=$!
sleep 1

# Adiciona 3 documentos
idx1=$(./bin/dclient -a "Doc1" "Autor1" 2021 "$DOC_DIR/doc1.txt" | grep -o '[0-9]\+')
idx2=$(./bin/dclient -a "Doc2" "Autor2" 2022 "$DOC_DIR/doc2.txt" | grep -o '[0-9]\+')
idx3=$(./bin/dclient -a "Doc3" "Autor3" 2023 "$DOC_DIR/doc3.txt" | grep -o '[0-9]\+')

# Consulta índice 1 duas vezes → HIT esperado na segunda
./bin/dclient -c "$idx1" >> "$LOG"
./bin/dclient -c "$idx1" >> "$LOG"

# Consulta índice 2 para manter na cache
./bin/dclient -c "$idx2" >> "$LOG"

# Adiciona doc3 (já foi adicionado acima) — para forçar remoção
# Consulta idx1 de novo → se foi removido, é reinserido
./bin/dclient -c "$idx1" >> "$LOG"

# Fecha servidor
./bin/dclient -f >> "$LOG"
wait $SERVER_PID

# Análise dos logs
echo "📊 Resultados:"
grep -E "\[CACHE\]" "$LOG"

echo
echo "✅ Fim do teste. Log completo em $LOG"
