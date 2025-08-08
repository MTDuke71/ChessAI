#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== TESTING BLACK CASTLING ===\n";
    
    // Make a neutral white move that shouldn't affect black castling
    Board::MoveState state;
    board.makeMove("a2-a3", state);
    
    std::cout << "After a2-a3 (black to move):\n";
    std::cout << "  Black can castle kingside: " << board.canCastleBK() << "\n";
    std::cout << "  Black can castle queenside: " << board.canCastleBQ() << "\n";
    
    // Check if the castling moves are being generated
    MoveGenerator gen;
    auto blackMoves = gen.generateAllMoves(board, false);
    
    bool ooGenerated = false;
    bool oooGenerated = false;
    
    for (auto move : blackMoves) {
        std::string moveStr = decodeMove(move);
        if (moveStr == "O-O") {
            ooGenerated = true;
            std::cout << "  O-O generated for black\n";
            std::cout << "  O-O legal for black: " << board.isMoveLegal("O-O") << "\n";
        }
        if (moveStr == "O-O-O") {
            oooGenerated = true;
            std::cout << "  O-O-O generated for black\n";
            std::cout << "  O-O-O legal for black: " << board.isMoveLegal("O-O-O") << "\n";
        }
    }
    
    if (!ooGenerated) std::cout << "  O-O NOT generated for black\n";
    if (!oooGenerated) std::cout << "  O-O-O NOT generated for black\n";
    
    // Let's check manually if black should be able to castle
    std::cout << "\nManual castling check:\n";
    std::cout << "  Black king on e8: " << ((board.getBlackKing() & (1ULL << 60)) ? "YES" : "NO") << "\n";
    std::cout << "  Black rook on a8: " << ((board.getBlackRooks() & (1ULL << 56)) ? "YES" : "NO") << "\n";
    std::cout << "  Black rook on h8: " << ((board.getBlackRooks() & (1ULL << 63)) ? "YES" : "NO") << "\n";
    
    // Check if squares between king and rook are clear
    uint64_t allPieces = board.getWhitePieces() | board.getBlackPieces();
    std::cout << "  b8 clear: " << (!(allPieces & (1ULL << 57)) ? "YES" : "NO") << "\n";
    std::cout << "  c8 clear: " << (!(allPieces & (1ULL << 58)) ? "YES" : "NO") << "\n";
    std::cout << "  d8 clear: " << (!(allPieces & (1ULL << 59)) ? "YES" : "NO") << "\n";
    std::cout << "  f8 clear: " << (!(allPieces & (1ULL << 61)) ? "YES" : "NO") << "\n";
    std::cout << "  g8 clear: " << (!(allPieces & (1ULL << 62)) ? "YES" : "NO") << "\n";
    
    return 0;
}
