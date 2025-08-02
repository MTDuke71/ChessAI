#include "Board.h"
#include "Engine.h"
#include <iostream>

int main() {
    Engine engine;
    Board connected, isolated, doubled;
    connected.loadFEN("4k3/8/8/8/2PP4/8/8/4K3 w - - 0 1");
    isolated.loadFEN("4k3/8/8/8/2P1P3/8/8/4K3 w - - 0 1");
    doubled.loadFEN("4k3/8/8/8/2P5/2P5/8/4K3 w - - 0 1");
    int scoreConnected = engine.evaluate(connected);
    int scoreIsolated = engine.evaluate(isolated);
    int scoreDoubled = engine.evaluate(doubled);
    if (scoreConnected <= scoreIsolated) {
        std::cerr << "Isolated pawn evaluation failed: " << scoreConnected
                  << " vs " << scoreIsolated << std::endl;
        return 1;
    }
    if (scoreConnected <= scoreDoubled) {
        std::cerr << "Doubled pawn evaluation failed: " << scoreConnected
                  << " vs " << scoreDoubled << std::endl;
        return 1;
    }
    return 0;
}
