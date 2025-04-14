#!/bin/bash
# Script to check if all files in a .tsv file exist in a specified directory and validate year format.
# Usage: ./checkTsvFiles.sh <metadata.tsv> <directory>

# Validate input
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <metadata.tsv> <directory>"
    exit 1
fi

TSV_FILE="$1"
TARGET_DIR="$2"

# Check if the TSV file exists
if [ ! -f "$TSV_FILE" ]; then
    echo "Error: File '$TSV_FILE' not found."
    exit 1
fi

# Check if the directory exists
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: Directory '$TARGET_DIR' not found."
    exit 1
fi

MISSING=0
TOTAL=0
INVALID_YEAR=0

echo "Checking files listed in '$TSV_FILE' against directory '$TARGET_DIR'..."
echo

# Skip the header line
while IFS=$'\t' read -r filename title year authors; do
    TOTAL=$((TOTAL + 1))

    # Check if the year is a valid number (only digits)
    if ! [[ "$year" =~ ^[0-9]+$ ]]; then
        echo "❌ Invalid year format: $year (Filename: $filename)"
        INVALID_YEAR=$((INVALID_YEAR + 1))
    fi

    # Check if file exists in the directory
    if [ ! -f "$TARGET_DIR/$filename" ]; then
        echo "❌ Missing: $filename"
        MISSING=$((MISSING + 1))
    fi

done < <(tail -n +2 "$TSV_FILE")

echo
echo "Check complete."
echo "Total files listed: $TOTAL"
echo "Missing files: $MISSING"
echo "Invalid year format: $INVALID_YEAR"
