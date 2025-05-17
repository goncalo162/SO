#!/bin/bash

# Script para remover todos os ficheiros cujo nome começa por "pipe"
# no diretório atual e em subdiretórios.

# Procura recursivamente por ficheiros com nome que começa por "pipe"
# e remove-os com o comando `rm`
find -name "pipe*" -exec rm {} \;