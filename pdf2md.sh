#!/bin/bash
# PDF to Markdown Converter
# Usage: ./pdf2md.sh input.pdf [output.md]

set -e

if [ $# -eq 0 ]; then
    echo "Usage: $0 <input.pdf> [output.md]"
    echo "Example: $0 document.pdf document.md"
    exit 1
fi

INPUT_PDF="$1"
OUTPUT_MD="${2:-${INPUT_PDF%.pdf}.md}"

if [ ! -f "$INPUT_PDF" ]; then
    echo "Error: File '$INPUT_PDF' not found"
    exit 1
fi

echo "Converting: $INPUT_PDF → $OUTPUT_MD"
echo "Extracting text from PDF..."

# Extract text from PDF and convert to markdown
pdftotext -layout "$INPUT_PDF" - | pandoc -f plain -t markdown -o "$OUTPUT_MD"

echo "✓ Conversion complete: $OUTPUT_MD"
echo ""
echo "Preview (first 20 lines):"
head -20 "$OUTPUT_MD"
