#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

// Let me create a simplified version of isMoveLegal with debug output
bool debugIsMoveLegal(const Board& board, uint16_t move) {
    int from = (move >> 6) & 0x3F;
    int to = move & 0x3F;
    int special = (move >> 14) & 0x03;
    
    std::cout << "DEBUG: Checking move legality:\n";
    std::cout << "  from=" << from << ", to=" << to << ", special=" << special << "\n";
    
    bool isWhite = board.isWhiteToMove();
    uint64_t fromMask = 1ULL << from;
    uint64_t toMask = 1ULL << to;
    uint64_t own = isWhite ? board.getWhitePieces() : board.getBlackPieces();
    uint64_t opp = isWhite ? board.getBlackPieces() : board.getWhitePieces();
    uint64_t occ = own | opp;
    
    std::cout << "  isWhite=" << isWhite << "\n";
    std::cout << "  fromMask has piece=" << ((own & fromMask) ? "YES" : "NO") << "\n";
    std::cout << "  toMask has piece=" << ((occ & toMask) ? "YES" : "NO") << "\n";
    
    // Check if there's a piece to move
    if (!(own & fromMask)) {
        std::cout << "  FAIL: No piece at from square\n";
        return false;
    }

    // For non-castling moves, check if moving onto own piece
    if (own & toMask) {
        std::cout << "  FAIL: Moving onto own piece\n";
        return false;
    }
    
    if (special == 3) {
        std::cout << "  This is a castling move\n";
        std::cout << "  castleWK=" << board.canCastleWK() << "\n";
        
        if (isWhite && from == 4 && to == 6) {
            std::cout << "  White kingside castling attempt\n";
            bool clear = !(occ & ((1ULL << 5) | (1ULL << 6)));
            std::cout << "  Path clear (f1,g1)=" << clear << "\n";
            
            if (board.canCastleWK() && clear) {
                std::cout << "  SUCCESS: Castling should be legal\n";
                return true;
            } else {
                std::cout << "  FAIL: Castling conditions not met\n";
                return false;
            }
        }
    }
    
    std::cout << "  FAIL: No valid move pattern found\n";
    return false;
}

int main() {
    std::cout << "=== DETAILED CASTLING LEGALITY DEBUG ===\n";
    
    Board board;
    board.loadFEN("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    
    uint16_t castlingMove = encodeMove("e1-g1");
    
    std::cout << "Testing e1-g1 castling move:\n";
    bool legal = debugIsMoveLegal(board, castlingMove);
    std::cout << "\nResult: " << (legal ? "LEGAL" : "ILLEGAL") << "\n";
    
    std::cout << "\nComparing with board.isMoveLegal:\n";
    bool boardLegal = board.isMoveLegal(castlingMove);
    std::cout << "board.isMoveLegal result: " << (boardLegal ? "LEGAL" : "ILLEGAL") << "\n";
    
    return 0;
}
