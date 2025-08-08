#include "../src/Board.h"
#include <iostream>

void debugEnPassantLogic() {
    std::cout << "=== DEBUGGING EN PASSANT LOGIC ===\n";
    
    // g2-g4 analysis
    int from = 14; // g2
    int to = 30;   // g4
    
    std::cout << "Move: g2-g4\n";
    std::cout << "from = " << from << " (g2)\n";
    std::cout << "to = " << to << " (g4)\n";
    std::cout << "abs(to - from) = " << std::abs(to - from) << " (should be 16)\n";
    
    int mid = (from + to) / 2;
    std::cout << "mid = " << mid << " (should be g3 = 22)\n";
    
    // Check adjacent squares
    std::cout << "to % 8 = " << (to % 8) << " (file of g4)\n";
    std::cout << "Left adjacent square: " << (to - 1) << " (should be f4 = 29)\n";
    std::cout << "Right adjacent square: " << (to + 1) << " (should be h4 = 31)\n";
    
    // But wait, the black pawn is on h3 (square 23), not h4!
    std::cout << "\nThe issue: black pawn is on h3 (23), not h4 (31)\n";
    std::cout << "The logic should check squares adjacent to the en passant square (mid),\n";
    std::cout << "not squares adjacent to the destination!\n";
    
    std::cout << "\nCorrect logic should check:\n";
    std::cout << "Left of en passant square: " << (mid - 1) << " (f3 = 21)\n";  
    std::cout << "Right of en passant square: " << (mid + 1) << " (h3 = 23)\n";
}

int main() {
    debugEnPassantLogic();
    return 0;
}
