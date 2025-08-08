#include "../src/Board.h"
#include "../src/MoveGenerator.h"
#include "../src/MoveEncoding.h"
#include <iostream>

int main() {
    Board board;
    board.loadFEN("4R3/8/3k4/8/8/8/8/4K3 w - - 4 3");
    
    std::cout << "=== MOVE ENCODING BUG INVESTIGATION ===" << std::endl;
    std::cout << "Testing moves e8-c8 and e8-g8 to find the special field bug" << std::endl;
    std::cout << std::endl;
    
    // Test the problematic moves
    uint16_t move1 = encodeMove("e8-c8");
    uint16_t move2 = encodeMove("e8-g8");
    
    std::cout << "Move e8-c8:" << std::endl;
    std::cout << "  Encoded: " << move1 << std::endl;
    std::cout << "  From: " << moveFrom(move1) << std::endl;
    std::cout << "  To: " << moveTo(move1) << std::endl;
    std::cout << "  Special: " << moveSpecial(move1) << std::endl;
    std::cout << "  Promotion: " << movePromotion(move1) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Move e8-g8:" << std::endl;
    std::cout << "  Encoded: " << move2 << std::endl;
    std::cout << "  From: " << moveFrom(move2) << std::endl;
    std::cout << "  To: " << moveTo(move2) << std::endl;
    std::cout << "  Special: " << moveSpecial(move2) << std::endl;
    std::cout << "  Promotion: " << movePromotion(move2) << std::endl;
    std::cout << std::endl;
    
    // Check if these are being interpreted as castling moves
    std::cout << "=== CASTLING INTERPRETATION CHECK ===" << std::endl;
    
    // Check the rook position and move interpretation
    std::cout << "Rook is at e8 (square " << algebraicToIndex("e8") << ")" << std::endl;
    std::cout << "Moving to c8 (square " << algebraicToIndex("c8") << ")" << std::endl;
    std::cout << "Moving to g8 (square " << algebraicToIndex("g8") << ")" << std::endl;
    
    // Manual check of why special != 0
    std::cout << std::endl;
    std::cout << "=== DEBUGGING THE BUG ===" << std::endl;
    std::cout << "The bug is in Board::isMoveLegal() line 470:" << std::endl;
    std::cout << "  if (special != 0) return false;" << std::endl;
    std::cout << "This rejects rook moves with non-zero special fields." << std::endl;
    
    if (moveSpecial(move1) != 0) {
        std::cout << "e8-c8 has special=" << moveSpecial(move1) << " (should be 0 for normal rook move)" << std::endl;
    }
    if (moveSpecial(move2) != 0) {
        std::cout << "e8-g8 has special=" << moveSpecial(move2) << " (should be 0 for normal rook move)" << std::endl;
    }
    
    return 0;
}
