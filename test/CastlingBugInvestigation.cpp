#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/Perft.h"
#include "../src/MoveEncoding.h"
#include "../src/Magic.h"
#include <iostream>
#include <vector>
#include <map>

int main() {
    Board board;
    board.loadFEN("4R3/8/3k4/8/8/8/8/4K3 w - - 4 3");
    
    std::cout << "=== CASTLING RIGHTS BUG INVESTIGATION ===" << std::endl;
    std::cout << "Position: 4R3/8/3k4/8/8/8/8/4K3 w - - 4 3" << std::endl;
    std::cout << "Investigating if castling rights interpretation is blocking moves" << std::endl;
    std::cout << std::endl;
    
    board.printBoard();
    
    // Check castling rights
    std::cout << "=== CASTLING RIGHTS CHECK ===" << std::endl;
    std::cout << "White King-side: " << (board.canCastleWK() ? "YES" : "NO") << std::endl;
    std::cout << "White Queen-side: " << (board.canCastleWQ() ? "YES" : "NO") << std::endl;
    std::cout << "Black King-side: " << (board.canCastleBK() ? "YES" : "NO") << std::endl;
    std::cout << "Black Queen-side: " << (board.canCastleBQ() ? "YES" : "NO") << std::endl;
    std::cout << "Current FEN: " << board.getFEN() << std::endl;
    
    MoveGenerator gen;
    auto moves = gen.generateLegalMoves(board, true); // White to move
    
    std::cout << std::endl;
    std::cout << "=== MOVE GENERATION ANALYSIS ===" << std::endl;
    std::cout << "Total legal moves: " << moves.size() << std::endl;
    std::cout << std::endl;
    
    // Separate king and rook moves
    std::vector<uint16_t> kingMoves, rookMoves;
    for (const auto& move : moves) {
        int from = moveFrom(move);
        if (from == 28) { // e1 = 4 (but our king is at e1 = 4)
            kingMoves.push_back(move);
        } else if (from == 60) { // e8 = 60
            rookMoves.push_back(move);
        }
    }
    
    std::cout << "King moves (" << kingMoves.size() << "):" << std::endl;
    for (const auto& move : kingMoves) {
        std::cout << "  " << decodeMove(move) << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Rook moves (" << rookMoves.size() << "):" << std::endl;
    for (const auto& move : rookMoves) {
        std::string moveStr = decodeMove(move);
        std::cout << "  " << moveStr;
        
        // Check if this is the missing move
        if (moveStr == "e8-c8" || moveStr == "e8-g8") {
            std::cout << " ⚠️ CRITICAL MOVE";
        }
        std::cout << std::endl;
    }
    
    // Check for the specific missing moves
    std::cout << std::endl;
    std::cout << "=== MISSING MOVE INVESTIGATION ===" << std::endl;
    
    // Manually check if e8-c8 should be legal
    std::cout << "Checking e8-c8:" << std::endl;
    uint16_t testMove1 = encodeMove("e8-c8");
    bool legal1 = board.isMoveLegal(testMove1);
    std::cout << "  e8-c8 is " << (legal1 ? "LEGAL" : "ILLEGAL") << std::endl;
    
    std::cout << "Checking e8-g8:" << std::endl;
    uint16_t testMove2 = encodeMove("e8-g8");
    bool legal2 = board.isMoveLegal(testMove2);
    std::cout << "  e8-g8 is " << (legal2 ? "LEGAL" : "ILLEGAL") << std::endl;
    
    if (!legal1 || !legal2) {
        std::cout << std::endl;
        std::cout << "🐛 BUG FOUND: Our engine incorrectly rejects legal rook moves!" << std::endl;
        
        // Let's check what's blocking these moves
        std::cout << std::endl;
        std::cout << "=== DEBUGGING MOVE VALIDATION ===" << std::endl;
        
        // Check the rook's attack pattern
        uint64_t rookAttacks = Magic::getRookAttacks(60, board.getWhitePieces() | board.getBlackPieces());
        std::cout << "Rook on e8 attacks squares: ";
        for (int sq = 0; sq < 64; ++sq) {
            if (rookAttacks & (1ULL << sq)) {
                char file = 'a' + (sq % 8);
                char rank = '1' + (sq / 8);
                std::cout << file << rank << " ";
            }
        }
        std::cout << std::endl;
        
        // Check if c8 and g8 are in the attack pattern
        bool c8_attacked = rookAttacks & (1ULL << 58); // c8
        bool g8_attacked = rookAttacks & (1ULL << 62); // g8
        std::cout << "c8 in rook attacks: " << (c8_attacked ? "YES" : "NO") << std::endl;
        std::cout << "g8 in rook attacks: " << (g8_attacked ? "YES" : "NO") << std::endl;
    }
    
    return 0;
}
