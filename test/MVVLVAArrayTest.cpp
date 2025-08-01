#include "MVVLVA.h"
#include <cassert>
#include <iostream>

void testMVVLVAOrdering() {
    using namespace MVVLVA;
    // Capturing a queen with a pawn should score higher than capturing a pawn with a queen
    assert(Table[Queen][Pawn] > Table[Pawn][Queen]);
    // Capturing a rook with a knight should be better than capturing a pawn with a knight
    assert(Table[Rook][Knight] > Table[Pawn][Knight]);
    // Among equal victims, a pawn attacker should score higher than a queen attacker
    assert(Table[Bishop][Pawn] > Table[Bishop][Queen]);
    std::cout << "[✔] MVV LVA table ordering verified\n";
}

int main() {
    testMVVLVAOrdering();
    std::cout << "All MVV LVA tests passed!\n";
    return 0;
}
