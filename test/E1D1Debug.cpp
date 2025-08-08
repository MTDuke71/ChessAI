#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    Board board;
    board.loadFEN(fen);
    
    std::cout << "=== TESTING e1-d1 MOVE ===\n";
    
    std::cout << "Is e1-d1 legal according to board.isMoveLegal()? " << (board.isMoveLegal("e1-d1") ? "YES" : "NO") << "\n";
    
    // Check what square d1 looks like from the black pieces
    std::cout << "\n=== ANALYZING d1 SQUARE ===\n";
    std::cout << "Checking if black pieces can attack d1...\n";
    
    // Check what pieces can attack d1
    MoveGenerator gen;
    auto blackMoves = gen.generateAllMoves(board, false); // Black moves
    
    bool d1Attacked = false;
    for (auto move : blackMoves) {
        int to = move & 0x3f; // Extract 'to' square from move encoding
        if (to == 3) { // d1 = square 3
            std::string moveStr = decodeMove(move);
            std::cout << "Black can attack d1 with: " << moveStr << "\n";
            d1Attacked = true;
        }
    }
    
    if (!d1Attacked) {
        std::cout << "No black pieces can attack d1\n";
    }
    
    return 0;
}
