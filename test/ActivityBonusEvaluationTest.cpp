#include "Board.h"
#include "Engine.h"
#include <iostream>

int main() {
    Engine engine;
    Board openFile, blockedFile, outpost, attacked;
    // Rook on open file vs rook blocked by enemy pawn, material kept equal
    openFile.loadFEN("4k3/1p6/8/8/8/8/8/R3K3 w - - 0 1");
    blockedFile.loadFEN("4k3/p7/8/8/8/8/8/R3K3 w - - 0 1");
    int openScore = engine.evaluate(openFile);
    int blockedScore = engine.evaluate(blockedFile);
    if (openScore <= blockedScore) {
        std::cerr << "Rook open file evaluation failed: " << openScore
                  << " vs " << blockedScore << std::endl;
        return 1;
    }
    // Knight outpost vs knight attacked by pawn
    outpost.loadFEN("4k3/8/8/3N4/4P3/8/8/4K3 w - - 0 1");
    attacked.loadFEN("4k3/8/2p5/3N4/4P3/8/8/4K3 w - - 0 1");
    int outpostScore = engine.evaluate(outpost);
    int attackedScore = engine.evaluate(attacked);
    if (outpostScore <= attackedScore) {
        std::cerr << "Knight outpost evaluation failed: " << outpostScore
                  << " vs " << attackedScore << std::endl;
        return 1;
    }
    return 0;
}
