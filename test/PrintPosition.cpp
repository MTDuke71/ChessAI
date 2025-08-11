#include "../src/Board.h"
#include <iostream>
#include <iomanip>

// Get piece character at square
char getPieceChar(const Board& board, int square) {
    uint64_t mask = 1ULL << square;
    if (board.getWhitePawns() & mask) return 'P';
    if (board.getWhiteKnights() & mask) return 'N';
    if (board.getWhiteBishops() & mask) return 'B';
    if (board.getWhiteRooks() & mask) return 'R';
    if (board.getWhiteQueens() & mask) return 'Q';
    if (board.getWhiteKing() & mask) return 'K';
    if (board.getBlackPawns() & mask) return 'p';
    if (board.getBlackKnights() & mask) return 'n';
    if (board.getBlackBishops() & mask) return 'b';
    if (board.getBlackRooks() & mask) return 'r';
    if (board.getBlackQueens() & mask) return 'q';
    if (board.getBlackKing() & mask) return 'k';
    return '.';
}

void printBoard(const Board& board) {
    std::cout << "\n    a   b   c   d   e   f   g   h\n";
    std::cout << "  +---+---+---+---+---+---+---+---+\n";
    
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " |";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char piece = getPieceChar(board, square);
            std::cout << " " << piece << " |";
        }
        std::cout << " " << rank + 1 << "\n";
        std::cout << "  +---+---+---+---+---+---+---+---+\n";
    }
    std::cout << "    a   b   c   d   e   f   g   h\n\n";
}

int main() {
    Board board;
    
    // The problematic position
    board.loadFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1");
    
    std::cout << "Position: r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1\n";
    std::cout << "White to move, all castling rights available\n";
    
    printBoard(board);
    
    std::cout << "Analysis:\n";
    std::cout << "- White king on e1, rooks on a1 and h1\n";
    std::cout << "- Black king on e8, rooks on a8 and h8\n";
    std::cout << "- All pieces on their starting ranks\n";
    std::cout << "- Black rooks attack the entire first rank (including e1, f1, g1)\n";
    std::cout << "- White rooks attack the entire eighth rank (including e8, f8, g8)\n";
    std::cout << "- Therefore, NO castling should be legal for either side!\n";
    std::cout << "- Kings would be moving through/into check\n\n";
    
    std::cout << "Expected moves: Only pawn moves and knight moves should be legal\n";
    std::cout << "- White: a2-a3, a2-a4, b2-b3, b2-b4, c2-c3, c2-c4, d2-d3, d2-d4\n";
    std::cout << "         e2-e3, e2-e4, f2-f3, f2-f4, g2-g3, g2-g4, h2-h3, h2-h4\n";
    std::cout << "         Nb1-a3, Nb1-c3, Ng1-f3, Ng1-h3\n";
    std::cout << "- Total: 16 pawn moves + 4 knight moves = 20 moves\n\n";
    
    return 0;
}
