#include "Board.h"
#include "Engine.h"
#include <iostream>

int main() {
    Engine engine;
    Board shielded, noShield, safe, attacked;

    // Pawn shield vs exposed king
    shielded.loadFEN("6k1/5ppp/8/8/8/8/5PPP/6K1 w - - 0 1");
    noShield.loadFEN("6k1/5ppp/8/8/8/5PPP/8/6K1 w - - 0 1");
    int shieldScore = engine.evaluate(shielded);
    int noShieldScore = engine.evaluate(noShield);
    if (shieldScore <= noShieldScore) {
        std::cerr << "Pawn shield evaluation failed: " << shieldScore
                  << " vs " << noShieldScore << std::endl;
        return 1;
    }

    // Exposed squares around king under attack
    safe.loadFEN("r5k1/5ppp/8/8/8/8/5PPP/R5K1 w - - 0 1");
    attacked.loadFEN("6k1/5p1p/8/8/8/8/5PrP/R5K1 w - - 0 1");
    int safeScore = engine.evaluate(safe);
    int attackedScore = engine.evaluate(attacked);
    if (safeScore <= attackedScore) {
        std::cerr << "Exposed king evaluation failed: " << safeScore
                  << " vs " << attackedScore << std::endl;
        return 1;
    }

    return 0;
}
