#!/bin/bash

# Script : addGdatasetMetadata.sh
# Descrição : Adiciona metadados de documentos usando o programa `dclient`,
#             lendo os dados a partir de um ficheiro TSV (como o Gcatalog).

# Verifica se foi fornecido exatamente um argumento (o ficheiro de entrada)
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <Gcatalog_file> "
    exit 1
fi

# Guarda o nome do ficheiro de entrada
INPUT_FILE="$1"

# Verifica se o ficheiro fornecido existe
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
fi

# Inicializa um contador para os documentos processados
COUNT=0

# Lê o ficheiro de entrada linha a linha, usando tabulação ('\t') como delimitador
# A primeira linha (cabeçalho) é ignorada com o comando 'tail -n +2'
while IFS=$'\t' read -r filename title year authors; do
    COUNT=$((COUNT + 1))

    # Mostra os metadados do documento a serem processados
    echo "------------------------"
    echo "Filename: $filename"
    echo "Title: $title"
    echo "Year: $year"
    echo "Authors: $authors"

    # Chama o programa dclient com os metadados extraídos
    ./bin/dclient -a "$title" "$authors" $year "$filename"

done < <(tail -n +2 "$INPUT_FILE")

# Mostra o número total de documentos processados
echo -e "\nAdded metadata for $COUNT files."