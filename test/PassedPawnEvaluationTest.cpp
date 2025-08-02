#include "Board.h"
#include "Engine.h"
#include <iostream>

int main() {
    Engine engine;
    Board passed, blocked;
    // White pawn on d5 with a black pawn far away -> passed pawn
    passed.loadFEN("4k3/8/8/3P4/8/8/8/p3K3 w - - 0 1");
    // Same material but black pawn on e6 blocking the pawn
    blocked.loadFEN("4k3/4p3/8/3P4/8/8/8/4K3 w - - 0 1");
    int scorePassed = engine.evaluate(passed);
    int scoreBlocked = engine.evaluate(blocked);
    if (scorePassed <= scoreBlocked) {
        std::cerr << "Passed pawn evaluation failed: " << scorePassed
                  << " vs " << scoreBlocked << std::endl;
        return 1;
    }
    return 0;
}
