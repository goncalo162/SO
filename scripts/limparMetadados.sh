#!/bin/bash

# Script para esvaziar (truncar) o ficheiro de metadados.
# Utiliza o comando `truncate` para definir o tamanho do ficheiro para 0 bytes.

# Trunca o ficheiro 'metadados', removendo todo o seu conteúdo
truncate -s 0 metadados