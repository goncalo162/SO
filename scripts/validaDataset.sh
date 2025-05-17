#!/bin/bash

# Script para verificar se todos os ficheiros listados num ficheiro .tsv
# existem num diretório especificado e se o campo "ano" tem o formato numérico válido.


# Verificação dos argumentos recebidos
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <metadata.tsv> <directory>"
    exit 1
fi

TSV_FILE="$1"  # Ficheiro .tsv com metadados
TARGET_DIR="$2" # Diretório onde os ficheiros devem existir

# Verifica se o ficheiro .tsv existe
if [ ! -f "$TSV_FILE" ]; then
    echo "Error: File '$TSV_FILE' not found."
    exit 1
fi

# Verifica se o diretório fornecido existe
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: Directory '$TARGET_DIR' not found."
    exit 1
fi

# Inicializa contadores
MISSING=0    # Contador de ficheiros em falta
TOTAL=0       # Total de entradas lidas
INVALID_YEAR=0   # Contador de anos com formato inválido

echo "Checking files listed in '$TSV_FILE' against directory '$TARGET_DIR'..."
echo

# Lê o ficheiro .tsv linha a linha, ignorando o cabeçalho
while IFS=$'\t' read -r filename title year authors; do
    TOTAL=$((TOTAL + 1))

    # Verifica se o campo "ano" contém apenas dígitos
    if ! [[ "$year" =~ ^[0-9]+$ ]]; then
        echo "❌ Invalid year format: $year (Filename: $filename)"
        INVALID_YEAR=$((INVALID_YEAR + 1))
    fi

    # Verifica se o ficheiro existe no diretório alvo
    if [ ! -f "$TARGET_DIR/$filename" ]; then # Ignora a primeira linha 
        echo "❌ Missing: $filename"
        MISSING=$((MISSING + 1))
    fi

done < <(tail -n +2 "$TSV_FILE")

# Resumo final
echo
echo "Check complete."
echo "Total files listed: $TOTAL"
echo "Missing files: $MISSING"
echo "Invalid year format: $INVALID_YEAR"
