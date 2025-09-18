#!/bin/bash

# Test script for README.md examples
# This script tests all the command-line converter examples mentioned in README.md

echo "🧪 Testing README.md Command Line Converter Examples"
echo "=================================================="

# Setup environment
export PATH="/opt/homebrew/opt/qt@5/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/qt@5/lib"
export CPPFLAGS="-I/opt/homebrew/opt/qt@5/include"
export PKG_CONFIG_PATH="/opt/homebrew/opt/qt@5/lib/pkgconfig"

# Test input file
INPUT_FILE="runtime/test.dxf"
TEST_DIR="test_output"

# Create test directory
mkdir -p "$TEST_DIR"

echo ""
echo "📋 Input file: $INPUT_FILE"
echo "📊 Input file size: $(du -h "$INPUT_FILE" | cut -f1)"

echo ""
echo "1️⃣ Testing G-code conversion (.ngc format)"
echo "-------------------------------------------"
./build/dxf_converter "$INPUT_FILE" "$TEST_DIR/test.ngc"
if [ $? -eq 0 ]; then
    echo "✅ G-code conversion successful"
    echo "   Output size: $(du -h "$TEST_DIR/test.ngc" | cut -f1)"
    echo "   Lines: $(wc -l < "$TEST_DIR/test.ngc")"
else
    echo "❌ G-code conversion failed"
fi

echo ""
echo "2️⃣ Testing dxfplot conversion (.dxfplot format)"
echo "-----------------------------------------------"
./build/dxf_converter "$INPUT_FILE" "$TEST_DIR/test.dxfplot"
if [ $? -eq 0 ]; then
    echo "✅ dxfplot conversion successful"
    echo "   Output size: $(du -h "$TEST_DIR/test.dxfplot" | cut -f1)"
    echo "   Lines: $(wc -l < "$TEST_DIR/test.dxfplot")"
else
    echo "❌ dxfplot conversion failed"
fi

echo ""
echo "3️⃣ Testing JSON format conversion (.json format)"
echo "------------------------------------------------"
./build/dxf_converter "$INPUT_FILE" "$TEST_DIR/test.json"
if [ $? -eq 0 ]; then
    echo "✅ JSON conversion successful"
    echo "   Output size: $(du -h "$TEST_DIR/test.json" | cut -f1)"
    echo "   Lines: $(wc -l < "$TEST_DIR/test.json")"
else
    echo "❌ JSON conversion failed"
fi

echo ""
echo "4️⃣ Testing default output (no extension specified)"
echo "--------------------------------------------------"
./build/dxf_converter "$INPUT_FILE" "$TEST_DIR/test_default"
if [ $? -eq 0 ]; then
    echo "✅ Default conversion successful"
    echo "   Output size: $(du -h "$TEST_DIR/test_default.ngc" 2>/dev/null | cut -f1 || echo 'File not found')"
else
    echo "❌ Default conversion failed"
fi

echo ""
echo "5️⃣ Testing JSON structure validation"
echo "------------------------------------"
if command -v python3 &> /dev/null; then
    python3 -c "
import json
import sys
try:
    with open('$TEST_DIR/test.json', 'r') as f:
        data = json.load(f)
    print('✅ JSON structure is valid')
    print(f'   Root keys: {list(data.keys())}')
    if 'task' in data and 'layers' in data['task']:
        layers = data['task']['layers']
        print(f'   Layers found: {len(layers)}')
        total_paths = sum(len(layer.get('ptr_wrapper', {}).get('data', {}).get('children', [])) for layer in layers)
        print(f'   Total paths: {total_paths}')
    else:
        print('   Warning: Expected task/layers structure not found')
except Exception as e:
    print(f'❌ JSON validation failed: {e}')
    sys.exit(1)
"
else
    echo "⚠️  Python3 not available, skipping JSON validation"
fi

echo ""
echo "📁 Generated test files:"
echo "----------------------"
ls -lh "$TEST_DIR/"

echo ""
echo "🏁 All tests completed!"
echo "======================"
