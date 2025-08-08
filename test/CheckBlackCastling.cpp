#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::cout << "=== CHECKING BLACK CASTLING AFTER E5-F7 ===\n";
    
    // Start with the test position
    Board board;
    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Initial position - Black castling rights:\n";
    std::cout << "Can castle kingside: " << (board.canCastleBK() ? "YES" : "NO") << "\n";
    std::cout << "Can castle queenside: " << (board.canCastleBQ() ? "YES" : "NO") << "\n";
    
    // Make the e5-f7 move (white knight captures pawn)
    Board::MoveState state;
    board.makeMove("e5-f7", state);
    
    std::cout << "\nAfter e5-f7 (white knight takes pawn on f7):\n";
    std::cout << "Can castle kingside: " << (board.canCastleBK() ? "YES" : "NO") << "\n";
    std::cout << "Can castle queenside: " << (board.canCastleBQ() ? "YES" : "NO") << "\n";
    
    // Check what's on f7 and around the king
    std::cout << "\nBoard analysis after e5-f7:\n";
    std::cout << "Black king position: " << __builtin_ctzll(board.getBlackKing()) << " (e8 = 60)\n";
    std::cout << "White knights: " << std::hex << board.getWhiteKnights() << std::dec << "\n";
    
    // f7 should have white knight, f8 should be empty for castling
    uint64_t f7 = 1ULL << 53;
    uint64_t f8 = 1ULL << 61;
    
    std::cout << "f7 has white knight: " << ((board.getWhiteKnights() & f7) ? "YES" : "NO") << "\n";
    // Check if f8 has any piece (should be empty for castling)
    bool f8Occupied = (board.getWhitePawns() | board.getWhiteKnights() | board.getWhiteBishops() | 
                       board.getWhiteRooks() | board.getWhiteQueens() | board.getWhiteKing() |
                       board.getBlackPawns() | board.getBlackKnights() | board.getBlackBishops() | 
                       board.getBlackRooks() | board.getBlackQueens() | board.getBlackKing()) & f8;
    std::cout << "f8 is empty: " << (f8Occupied ? "NO" : "YES") << "\n";
    
    // Try to manually check if e8-g8 should be legal
    MoveGenerator gen;
    auto moves = gen.generateAllMoves(board, false);
    
    std::cout << "\nLooking for castling moves in generated moves:\n";
    bool foundKingside = false;
    bool foundQueenside = false;
    
    for (auto move : moves) {
        std::string moveStr = decodeMove(move);
        if (!board.isMoveLegal(moveStr)) continue;
        
        if (moveStr == "O-O" || moveStr == "e8-g8") {
            foundKingside = true;
            std::cout << "Found kingside castling: " << moveStr << "\n";
        }
        if (moveStr == "O-O-O" || moveStr == "e8-c8") {
            foundQueenside = true;
            std::cout << "Found queenside castling: " << moveStr << "\n";
        }
    }
    
    if (!foundKingside) {
        std::cout << "❌ Kingside castling NOT found in generated moves\n";
    }
    if (!foundQueenside) {
        std::cout << "❌ Queenside castling NOT found in generated moves\n";
    }
    
    return 0;
}
