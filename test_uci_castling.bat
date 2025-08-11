# Test UCI castling fix
# This script simulates a UCI session where the engine should generate castling moves

echo "Testing UCI castling move output..."
echo ""

# Test with a position where castling is available
cat << 'EOF'
uci
isready
position startpos moves e2e4 e7e5 g1f3 b8c6 f1b5 g8f6 e1g1 f8e7 f1e1 d7d6 c2c3 c8d7 d2d4 e5d4 c3d4 a7a6 b5c6 d7c6
go depth 5
EOF
